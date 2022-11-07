#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>

using namespace QSFML;
using namespace QSFML::Objects;
using namespace NeuronalNet;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CanvasSettings settings;
    //settings.timing.frameTime = 100;
    //settings.layout.autoAjustSize = false;
    settings.layout.fixedSize = sf::Vector2u(300,100);
    settings.contextSettings.antialiasingLevel = 8;
    m_canvas = new Canvas(ui->canvasWidget,settings);

    DefaultEditor *defaultEditor = new DefaultEditor();

    m_canvas->addObject(defaultEditor);

    m_model = new AI_Model();
    connect(ui->update_pushButton, &QPushButton::pressed,
            m_model, &AI_Model::onUpdatePressed);
    m_canvas->addObject(m_model);

}

MainWindow::~MainWindow()
{
    delete ui;
}

