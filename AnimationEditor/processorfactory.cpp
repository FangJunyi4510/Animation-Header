#include "processorfactory.h"

QMap<QString, std::function<Processor*()>> ProcessorFactory::creators;

void ProcessorFactory::registerByName(const QString &name, std::function<Processor *()> creator) {
    creators[name] = creator;
}

Processor *ProcessorFactory::create(const QString &name) {
    auto it = creators.find(name);
    if(it != creators.end()) {
        return it.value()();
    }
    return nullptr;
}
