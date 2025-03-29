#ifndef PROCESSORFACTORY_H
#define PROCESSORFACTORY_H
#include "processor.h"
#include <Qt>
#include <QMap>

class ProcessorFactory {
    static QMap<QString, std::function<Processor*()>> creators;
public:
    static void registerByName(const QString &name, std::function<Processor*()> creator);
    static Processor* create(const QString& name);
};

#endif // PROCESSORFACTORY_H
