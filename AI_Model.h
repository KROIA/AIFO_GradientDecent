#pragma once

#include "neuronalNet.h"
#include "SFML_EditorWidget.h"
#include "netModel.h"

class AI_Model: public QObject, public QSFML::Objects::CanvasObject
{
    Q_OBJECT
public:
    struct TrainingData
    {
        std::vector<float> input;
        std::vector<float> desiredOutput;
    };

    AI_Model();
    ~AI_Model();

    void setupDataset();
    float targetFunction(float x, size_t outputIndex = 0);

    void plotResults();

    void update() override;
public slots:
    void onUpdatePressed();
private:
    NeuronalNet::BackpropNet *m_net;
    NeuronalNet::Graphics::NetModel *m_visualModel;
    QSFML::Objects::LineChart * m_trainingDataInputChart;
    std::vector<QSFML::Objects::LineChart *> m_trainingDataDesiredCharts;
    std::vector<QSFML::Objects::LineChart *> m_predictedDataCharts;
    std::vector<QSFML::Objects::LineChart *> m_predictedDataCharts2;

    std::vector<TrainingData> m_trainingSet;
    bool m_training;
    size_t m_trainingIterations;
    size_t m_trainingIterationCounter;
};
