#include "AI_Model.h"
#include <QDebug>
#include <QFile>


AI_Model::AI_Model()
{
    m_net = new NeuronalNet::BackpropNet();
    m_net->setDimensions(1,1,3,3);
    m_net->setHardware(NeuronalNet::Hardware::cpu);
    m_net->setLearnParameter(0.01);
    m_net->setActivation(NeuronalNet::Activation::sigmoid);
    m_net->build();


    m_visualModel = new NeuronalNet::Graphics::NetModel(m_net);
    m_visualModel->setPos(sf::Vector2f(500,50));
    m_trainingDataInputChart = new QSFML::Objects::LineChart();
    m_trainingDataInputChart->setOrigin(sf::Vector2f(0,350));
    m_trainingDataInputChart->setScale(300);
    m_trainingDataInputChart->setSize(sf::Vector2f(400,400));
    m_trainingDataInputChart->setColor(sf::Color::Green);

    for(size_t i = 0; i<m_net->getOutputCount(); ++i)
    {
        QSFML::Objects::LineChart *desired = new QSFML::Objects::LineChart(*m_trainingDataInputChart);
        QSFML::Objects::LineChart *predicted = new QSFML::Objects::LineChart(*m_trainingDataInputChart);
        m_trainingDataDesiredCharts.push_back(desired);
        m_predictedDataCharts.push_back(predicted);
        desired->setColor(sf::Color(0,0,255));
        predicted->setColor(sf::Color(100,100,255));

        addChild(desired);
        addChild(predicted);


    }


    m_visualModel->setNeuronSize(15);
    m_visualModel->setNeuronSpacing(sf::Vector2f(20,1));
    m_visualModel->setConnectionWidth(5);

    addChild(m_visualModel);
    //addChild(m_trainingDataInputChart);
    //addChild(m_trainingDataDesiredChart);
    //addChild(m_predictedDataChart);

    m_training = true;
    m_trainingIterations = 1000;
    m_trainingIterationCounter = 0;

    setupDataset();
}
AI_Model::~AI_Model()
{
    delete m_visualModel;
    delete m_net;
}

void AI_Model::setupDataset()
{
    float minRange = -5;
    float maxRange = 5;
    float noiseRadius = 0.01;
    float xStepsize = 0.02;

    float x = minRange;
    m_trainingSet.clear();
    std::vector<float> plotInputPoints;
    std::vector<std::vector<float> >plotDesiredPoints(m_net->getOutputCount());
    while(x <= maxRange)
    {
        TrainingData dataPoint;
        dataPoint.input.push_back(x);
        for(size_t i=0; i<m_net->getOutputCount(); ++i)
        {
            float targetY = targetFunction(x, i);
            float noise = 0;
            if(noiseRadius != 0)
                noise = (float)(rand()%size_t(1000*noiseRadius*2))/1000.f - noiseRadius;
            //float noise = NeuronalNet::Net::getRandomValue(-noiseRadius,noiseRadius);


            dataPoint.desiredOutput.push_back(targetY+noise);


            plotInputPoints.push_back(dataPoint.input[i]);
            plotDesiredPoints[i].push_back(dataPoint.desiredOutput[i]);
        }
       /* if(m_net->getOutputCount() > 1)
        {
            float targetY = targetFunction(x+1);
            float noise = (float)(rand()%size_t(1000*noiseRadius*2))/1000.f - noiseRadius;
            //float noise = NeuronalNet::Net::getRandomValue(-noiseRadius,noiseRadius);
           // TrainingData dataPoint;
            //dataPoint.input.push_back(x);
            dataPoint.desiredOutput.push_back(targetY+noise);
            //m_trainingSet.push_back(dataPoint);
            //x += xStepsize;

            //plotInputPoints[1].push_back(dataPoint.input[0]);
            plotDesiredPoints[1].push_back(dataPoint.desiredOutput[1]);
        }*/
        m_trainingSet.push_back(dataPoint);
        x += xStepsize;
        //qDebug() << noise;
    }

    for(size_t i=0; i<m_trainingDataDesiredCharts.size(); ++i)
    {
        if(i >= plotDesiredPoints.size())
            continue;
        m_trainingDataDesiredCharts[i]->setDataPoints(plotDesiredPoints[i]);
    }
    m_trainingDataInputChart->setDataPoints(plotInputPoints);
   // m_trainingDataDesiredChart->setDataPoints(plotDesiredPoints);

}
float AI_Model::targetFunction(float x, size_t outputIndex)
{
    float yScale = 0.5;
    switch(outputIndex)
    {
        case 0:
        {
            if(x < 0.5)
                return -yScale;
            return yScale;
            //return sin(x)*0.7;
        }
        case 1:
        {
            return sin(x+2)*yScale;
        }
        case 2:
        {
            return abs(x)*yScale*0.1 - 0.3f;
        }
        default:
            return sin(x)*yScale*0.3;
    }
}

void AI_Model::plotResults()
{
    std::vector<std::vector<std::string> > lines;
    float minRange = -5;
    float maxRange = 5;
    float xStepsize = 0.05;
    float x = minRange;

    lines.push_back({"target","predicted"});
    while(x <= maxRange)
    {
        float targetY = targetFunction(x);

        std::string target = std::to_string(targetY);
        m_net->setInput(0,x);
        m_net->calculate();
        std::string predicted = std::to_string(m_net->getOutputVector()[0]);
        lines.push_back({target, predicted});

        x += xStepsize;
    }

    QFile file("result.csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "can't write to file";
        return;
    }
    QTextStream out(&file);
    for(size_t i=0; i<lines.size(); ++i)
    {
        for(size_t x=0; x<lines[i].size(); ++x)
        {
            out << lines[i][x].c_str() << ";";
        }
        out << "\n";
    }
    file.close();

    if(m_trainingSet.size() == 0)
        return;
    QFile fileTraining("trainingData.csv");
    if (!fileTraining.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "can't write to file";
        return;
    }
    QTextStream outTraining(&fileTraining);
    for(size_t x=0; x<m_trainingSet[0].input.size(); ++x)
    {
        outTraining << "inp_"<<x<<";";
    }
    outTraining<<";";
    for(size_t x=0; x<m_trainingSet[0].desiredOutput.size(); ++x)
    {
        outTraining << "OUT_"<<x<<";";
    }
    outTraining<<";";
    for(size_t x=0; x<m_trainingSet[0].desiredOutput.size(); ++x)
    {
        outTraining << "NET_"<<x<<";";
    }
    outTraining<<"\n";
    for(size_t i=0; i<m_trainingSet.size(); ++i)
    {
        for(size_t x=0; x<m_trainingSet[i].input.size(); ++x)
        {
            outTraining << m_trainingSet[i].input[x] << ";";
        }
        outTraining << ";";
        for(size_t x=0; x<m_trainingSet[i].desiredOutput.size(); ++x)
        {
            outTraining << m_trainingSet[i].desiredOutput[x] << ";";
        }
        outTraining << ";";
        m_net->setInputVector(m_trainingSet[i].input);
        m_net->calculate();
        NeuronalNet::SignalVector out = m_net->getOutputVector();
        for(size_t x=0; x<out.size(); ++x)
        {
            outTraining << out[x] << ";";
        }
        outTraining << "\n";
    }
    fileTraining.close();
}

void AI_Model::update()
{
    if(!m_training)
        return;
    if(m_trainingSet.size() == 0)
    {
        qDebug() << "Dataset is empty";
        return;
    }
    float netError = 0;
    std::vector<std::vector<float>> plotOutputPoints(m_net->getOutputCount());

    netError = 0;

    for(size_t i=0; i<m_trainingSet.size(); ++i)
    {
        m_net->setInputVector(m_trainingSet[i].input);
        m_net->calculate();
        //m_net->learn(m_trainingSet[i].desiredOutput);
        netError += m_net->getError().getRootMeanSquare();



        NeuronalNet::SignalVector outVec = m_net->getOutputVector();
        for(size_t out=0; out<m_net->getOutputCount(); ++out)
        {
            plotOutputPoints[out].push_back(outVec[out]);
        }

    }
    netError /=(float)m_trainingSet.size();


    for(size_t out=0; out<m_net->getOutputCount(); ++out)
        m_predictedDataCharts[out]->setDataPoints(plotOutputPoints[out]);


    for(size_t i=0; i<m_trainingSet.size(); ++i)
    {
        m_net->setInputVector(m_trainingSet[i].input);
        m_net->calculate();
        m_net->learn(m_trainingSet[i].desiredOutput);



        //netError += m_net->getError().getRootMeanSquare();
    }
    ++m_trainingIterationCounter;

    qDebug() << "Iteration" << m_trainingIterationCounter << " Net Error: "<<netError;
    if(m_trainingIterations <= m_trainingIterationCounter)
    {
        m_training = false;
        plotResults();
    }
    setupDataset();
}
void AI_Model::onUpdatePressed()
{
    m_training = true;
    m_trainingIterationCounter = 0;
    setupDataset();
}
