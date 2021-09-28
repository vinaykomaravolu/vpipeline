# vpipeline
Header only generic pipeline.

## Installation

1. git clone https://github.com/vinaykomaravolu/vpipeline.git
2. mkdir build && cd build
3. cmake .. && make

## Tests

* Test executable found in build/test/Pipeline_test.exe
* Tests found in test/pipeline-test.cpp

## Basic Example

```
class Transform : public Component
{
public:
    Transform()
    {
        position = vector<float>(3, 0.0f);
        rotation = vector<float>(3, 0.0f);
        scale = vector<float>(3, 0.0f);
    }

    vector<float> position;
    vector<float> rotation;
    vector<float> scale;
};

class GameObject : public ComponentsContainer
{
};

int main(){
    Pipeline p; // Create Pipeline
    shared_ptr<GameObject> g(new GameObject());
    shared_ptr<Transform> t(new Transform());
    (*g)["Transform"] = t; 
    p.pushPipelineNode("Move", [](shared_ptr<ComponentsContainer> c)
                       {
                           if (c->count("Transform"))
                           {
                               shared_ptr<Transform> t = static_pointer_cast<Transform>((*c)["Transform"]);
                               t->position[0] = 10.f;
                               t->position[2] = 5.f;
                           }
                       }); 
    p.setInput(g);         
    shared_ptr<GameObject> result = static_pointer_cast<GameObject>(p.process());
    shared_ptr<Transform> tresult = static_pointer_cast<Transform>((*result)["Transform"]);
}
```


## Class/Function Definitions

* enum PipelineResult
  * Success = 1
  * Fail = 0
* class Component
  * virtual ostream &print(ostream &o) const
  * friend ostream &operator<<(ostream &o, const Component &c)
* class ComponentsContainer
  * friend ostream &operator<<(ostream &o, const ComponentsContainer &c)
  * ComponentsContainer()
  * ComponentsContainer(const ComponentsContainer &c)
* class PipelineNode
  * PipelineNode()
  * PipelineNode(string id, function<void(shared_ptr<ComponentsContainer>)> func)
  * PipelineNode(string id, function<void(shared_ptr<ComponentsContainer>)> func, PipelineNode *next)
* class Pipeline
  * Pipeline()
  * ~Pipeline()
  * void setInput(shared_ptr<ComponentsContainer> cc)
  * shared_ptr<ComponentsContainer> process()
  * PipelineResult pushPipelineNode(string id, function<void(shared_ptr<ComponentsContainer>)> func)
  * void popPipelineNode()
  * PipelineResult removePipelineNode(string id)


