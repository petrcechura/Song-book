#ifndef TASKBASE_H
#define TASKBASE_H

class TaskBase {
  public:
    virtual ~TaskBase() = default;

    virtual int ParseCommand(std::string cmd_line) = 0;
    virtual int Execute(char command) = 0;
    virtual std::string GetName() = 0;
    virtual std::string GetDescription() = 0;
    //virtual void ChangeConfig(nlohmann::json& config) = 0;
};

#endif