#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "VisibleCamera.h"
#include "BackgroundGrid.h"
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

    VisibleCamera *cam      = new VisibleCamera("Camera");
    BackgroundGrid *grid    = new BackgroundGrid("Grid");
    CanvasObject *obj       = new CanvasObject("Test");

    grid->setSize(sf::IntRect(0,0,1800,1600));
    grid->setLineColor({sf::Color(130,130,130),
                        sf::Color(100,100,100),
                        sf::Color(100,100,100),
                        sf::Color(100,100,100),
                        sf::Color(100,100,100)});
    cam->setMaxMovingBounds(sf::FloatRect(grid->getSize()));

    obj->addChild(grid);
    obj->addChild(cam);

    m_canvas->addObject(obj);

    m_model = new AI_Model();
    connect(ui->update_pushButton, &QPushButton::pressed,
            m_model, &AI_Model::onUpdatePressed);
    m_canvas->addObject(m_model);

}

MainWindow::~MainWindow()
{
    delete ui;
}

