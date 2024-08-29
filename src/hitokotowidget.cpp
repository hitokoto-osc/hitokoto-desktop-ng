#include "hitokotowidget.h"

#include "conf.h"
#include "ui_HitokotoWidget.h"
#include <QFontDatabase>
#include <QMenu>
#include <QMouseEvent>
#include <QTimer>
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>

#define API_URL "https://v1.hitokoto.cn"

inline size_t assembly_stream(void* ptr, size_t size, size_t nmemb, void* stream)
{
    std::string* str = (std::string*)stream;
    (*str).append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

void HitokotoWidget::getHitokotoByNetwork()
{
    CURL* curl = curl_easy_init();
    if (!curl) {
        return;
    }

    std::string buffer;

    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, assembly_stream);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buffer);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);

    if (auto exec = curl_easy_perform(curl); exec != CURLE_OK) {
        std::cerr << "error " << curl_easy_strerror(exec) << std::endl;
        curl_easy_cleanup(curl);
        return;
    }

    std::cout << buffer << std::endl;

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
        curl_easy_cleanup(curl);
        return;
    }

    nlohmann::json jsonData = nlohmann::json::parse(buffer);
    if (jsonData == nullptr) {
        curl_easy_cleanup(curl);
        return;
    }

    from = "";
    fromWho = "";
    creator = "";
    text = "";

    if (jsonData["from_who"] != nullptr) {
        fromWho = std::string(jsonData["from_who"]);
    }

    if (jsonData["from"] != nullptr) {
        std::string ori(jsonData["from"]);
        if (fromWho == ori) {
            fromWho = "";
        }

        from = std::format("「{}」", ori);
    }

    if (jsonData["creator"] != nullptr) {
        creator = std::string(jsonData["creator"]);
    }

    if (jsonData["hitokoto"] != nullptr) {
        text = std::string(jsonData["hitokoto"]);
    }
}

HitokotoWidget::HitokotoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HitokotoWidget)
{
    ui->setupUi(this);

    // 设置 WindowStaysOnTopHint 且未设置 Qt::WA_StyledBackground 时无法实现拖拽
    // 此时只能在鼠标移出边缘之后，再将窗口置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    // 设置背景透明，但是有背景
    setAttribute(Qt::WA_StyledBackground);
    // 初始化任务栏图标
    initTray();

    currentGeometry = geometry();
    inGeometryPressed = false;

    int fontID = QFontDatabase::addApplicationFont(":/LXGWWenKaiMono-Regular.ttf");

    QFont font;
    font.setFamilies(QFontDatabase::applicationFontFamilies(fontID));
    font.setPointSize(35);
    ui->contents->setFont(font);

    QFont font1;
    font1.setFamilies(QFontDatabase::applicationFontFamilies(fontID));
    font1.setPointSize(18);
    ui->from->setFont(font1);

    if (bool loaded = loadCache(); loaded) {
        this->refreshHitokotoFromNetwork();
    }

    renderText();

    timer = new QTimer;
    connect(timer, &QTimer::timeout, [this] { this->refreshHitokotoFromNetwork(); });
    timer->start(1000 * 20);
}

void HitokotoWidget::initTray()
{
    tray = new QSystemTrayIcon();
    tray->setToolTip("Hitokoto On Desktop");
    tray->setIcon(QIcon(":/logo.png"));
    tray->show();

    QMenu menu;
    QAction* quit = new QAction("Exit");
    menu.addAction(quit);

    tray->setContextMenu(&menu);
    connect(quit, &QAction::triggered, this, [this]() { exit(0); });
}

bool HitokotoWidget::loadCache()
{
    auto cache = conf::instance().core()->FirstChildElement("last_hitokoto");
    if (auto c = cache->FirstChildElement("text")->FirstChild(); c != nullptr) {
        text = c->Value();
    }

    if (auto c = cache->FirstChildElement("creator")->FirstChild(); c != nullptr) {
        creator = c->Value();
    }

    if (auto c = cache->FirstChildElement("from")->FirstChild(); c != nullptr) {
        from = c->Value();
    }

    if (auto c = cache->FirstChildElement("from_who")->FirstChild(); c != nullptr) {
        fromWho = c->Value();
    }

    return text.empty();
}

void HitokotoWidget::renderText()
{
    ui->contents->setText("");
    ui->from->setText("");
    repaint();

    ui->contents->setText(text.c_str());
    ui->from->setText(std::format("—— {}{}", fromWho, from).c_str());

    ui->contents->repaint();
    ui->from->repaint();
}

void HitokotoWidget::refreshHitokotoFromNetwork()
{
    this->getHitokotoByNetwork();
    this->renderText();

    auto cache = conf::instance().core()->FirstChildElement("last_hitokoto");
    cache->FirstChildElement("text")->SetText(text.c_str());
    cache->FirstChildElement("creator")->SetText(creator.c_str());
    cache->FirstChildElement("from")->SetText(from.c_str());
    cache->FirstChildElement("from_who")->SetText(fromWho.c_str());
    conf::instance().update_core();
}

void HitokotoWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        mousePosition = e->pos();
        inGeometryPressed = currentGeometry.contains(mousePosition);
    }
}

void HitokotoWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (inGeometryPressed) {
        move(pos() + e->pos() - mousePosition);
    }
}

bool HitokotoWidget::event(QEvent* event)
{
    if (event->type() == QEvent::Enter) {
        setStyleSheet("background-color: rgba(1, 1, 1, 0.1);");
    }

    if (event->type() == QEvent::Leave) {
        setStyleSheet("background-color: rgba(1, 1, 1, 0);");
    }

    return QWidget::event(event);
}

void HitokotoWidget::mouseReleaseEvent(QMouseEvent*) { inGeometryPressed = false; }

HitokotoWidget::~HitokotoWidget() { delete ui; }
