#include <iostream>
#include <string>
#include <set>
#include <functional>
#include <unordered_map>
#include <memory>

using namespace std;

enum PipelineResult
{
    Success = 1,
    Fail = 0,
};

class Component
{
public:
    virtual ostream &print(ostream &o) const
    {
        return o;
    }

    friend ostream &operator<<(ostream &o, const Component &c)
    {
        return c.print(o);
    }
};

class ComponentsContainer : public unordered_map<string, shared_ptr<Component>>
{
public:
    friend ostream &operator<<(ostream &o, const ComponentsContainer &c)
    {
        for (auto it = c.begin(); it != c.end(); ++it)
        {
            o << "****" << it->first << "****\n"
              << *(it->second) << endl;
        }
        return o;
    }

    ComponentsContainer()
    {
    }

    ComponentsContainer(const ComponentsContainer &c)
    {
        for (auto it = c.begin(); it != c.end(); ++it)
        {
            (*this)[it->first] = it->second;
        }
    }

    void setComponent(string id, shared_ptr<Component> c)
    {
        this->insert(make_pair(id, c));
    }

    template <typename T>
    shared_ptr<T> getComponent(string id)
    {
        return static_pointer_cast<T>(this->at(id));
    }

    void removeComponent(string id)
    {
        this->erase(id);
    }
};

class PipelineNode
{
public:
    string id;
    function<void(shared_ptr<ComponentsContainer>)> func;
    PipelineNode *next = nullptr;
    PipelineNode *prev = nullptr;
    PipelineNode(){};
    PipelineNode(string id, function<void(shared_ptr<ComponentsContainer>)> func) : func(func), id(id){};
    PipelineNode(string id, function<void(shared_ptr<ComponentsContainer>)> func, PipelineNode *next) : func(func), next(next){};
};

class Pipeline
{
public:
    Pipeline()
    {
        head = new PipelineNode();
        tail = new PipelineNode();
        head->next = tail;
        tail->prev = head;
    }

    ~Pipeline()
    {
        PipelineNode *curr = head->next;
        while (curr != tail)
        {
            PipelineNode *temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    void setInput(shared_ptr<ComponentsContainer> cc)
    {
        this->cc = cc;
        ccSet = true;
    }

    template <typename T = ComponentsContainer>
    shared_ptr<T> process()
    {
        if (!ccSet)
        {
            throw "Pipeline Input not set";
        }
        shared_ptr<ComponentsContainer> processed(new ComponentsContainer(*cc));
        PipelineNode *curr = head->next;
        while (curr != tail)
        {
            curr->func(processed);
            curr = curr->next;
        }
        return static_pointer_cast<T>(processed);
    }

    PipelineResult pushPipelineNode(string id, function<void(shared_ptr<ComponentsContainer>)> func)
    {
        if (pipelineNodes.count(id))
        {
            return PipelineResult::Fail;
        }
        pipelineNodes[id] = new PipelineNode(id, func);
        PipelineNode *temp = tail->prev;
        temp->next = pipelineNodes[id];
        pipelineNodes[id]->prev = temp;
        pipelineNodes[id]->next = tail;
        tail->prev = pipelineNodes[id];
        return PipelineResult::Success;
    }

    void popPipelineNode()
    {
        PipelineNode *curr = tail->prev;
        if (curr != head)
        {
            curr->prev->next = curr->next;
            curr->next->prev = curr->prev;
            pipelineNodes.erase(curr->id);
            delete curr;
        }
    }

    PipelineResult removePipelineNode(string id)
    {
        if (pipelineNodes.count(id))
        {
            PipelineNode *curr = pipelineNodes[id];
            curr->prev->next = curr->next;
            curr->next->prev = curr->prev;
            delete curr;
            pipelineNodes.erase(id);
            return PipelineResult::Success;
        }
        return PipelineResult::Fail;
    }

    int size() const
    {
        return pipelineNodes.size();
    }

private:
    unordered_map<string, PipelineNode *>
        pipelineNodes;
    PipelineNode *head;
    PipelineNode *tail;
    shared_ptr<ComponentsContainer> cc;
    bool ccSet = false;
};
