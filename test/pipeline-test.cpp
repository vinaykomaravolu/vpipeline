#include "gtest/gtest.h"
#include "pipeline.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <memory>

using namespace std;

// Example components for tests
class Transform : public Component
{
public:
    Transform()
    {
        position = vector<float>(3, 0.0f);
        rotation = vector<float>(3, 0.0f);
        scale = vector<float>(3, 0.0f);
    }

    // Overload print
    ostream &print(ostream &o) const
    {
        o << "Position: " << position[0] << " " << position[1] << " " << position[2] << endl;
        o << "Rotation: " << rotation[0] << " " << rotation[1] << " " << rotation[2] << endl;
        o << "Scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
        return o;
    }

    vector<float> position;
    vector<float> rotation;
    vector<float> scale;
};

class Rigidbody : public Component
{
public:
    Rigidbody()
    {
        mass = 0.0f;
        drag = 0.0f;
        angularDrag = 0.0f;
        gravity = false;
    }
    float mass;
    float drag;
    float angularDrag;
    bool gravity;
};

class Info : public Component
{
public:
    Info(string desc) : desc(desc) {}
    string getInfo()
    {
        return desc;
    }

    void setInfo(string d)
    {
        desc = d;
    }

    ostream &print(ostream &o) const
    {
        o << desc << endl;
        return o;
    }

private:
    string desc;
};

class GameObject : public ComponentsContainer
{
};

TEST(PipelineTest, Initialize)
{
    Pipeline p;
    PipelineNode pn("test", [](shared_ptr<ComponentsContainer> c) {});
    ComponentsContainer cc();
    Component c;
    EXPECT_EQ(0, 0);
}

TEST(PipelineTest, AddComponents)
{
    Pipeline p;
    shared_ptr<GameObject> g(new GameObject());
    shared_ptr<Transform> t(new Transform());
    shared_ptr<Rigidbody> r(new Rigidbody());
    shared_ptr<Info> i(new Info("Testing game object"));
    (*g)["Transform"] = t;
    (*g)["Rigidbody"] = r;
    (*g)["Info"] = i;
    EXPECT_EQ(3, g->size());
}

TEST(PipelineTest, SingleComponent)
{
    Pipeline p; // Create Pipeline
    shared_ptr<GameObject> g(new GameObject());
    shared_ptr<Transform> t(new Transform());
    (*g)["Transform"] = t; // Fill up ComponentContainers
    EXPECT_EQ(1, g->size());
    p.pushPipelineNode("Move", [](shared_ptr<ComponentsContainer> c)
                       {
                           cout << *c << endl;
                           if (c->count("Transform"))
                           {
                               shared_ptr<Transform> t = static_pointer_cast<Transform>((*c)["Transform"]);
                               t->position[0] = 10.f;
                               t->position[2] = 5.f;
                           }
                       }); // Add pipeline function work
    p.setInput(g);         // Set pipeline input
    shared_ptr<GameObject> result = static_pointer_cast<GameObject>(p.process());
    EXPECT_EQ(true, result != NULL);
    shared_ptr<Transform> tresult = static_pointer_cast<Transform>((*result)["Transform"]);
    EXPECT_FLOAT_EQ(10.f, tresult->position[0]);
    EXPECT_FLOAT_EQ(5.f, tresult->position[2]);
}

TEST(PipelineTest, InputNotModifiedCheck)
{
    Pipeline p; // Create Pipeline
    shared_ptr<GameObject> g(new GameObject());
    shared_ptr<Transform> t(new Transform());
    (*g)["Transform"] = t; // Fill up ComponentContainers
    EXPECT_EQ(1, g->size());
    p.pushPipelineNode("Move", [](shared_ptr<ComponentsContainer> c)
                       {
                           if (c->count("Transform"))
                           {
                               shared_ptr<Transform> t = static_pointer_cast<Transform>((*c)["Transform"]);
                               t->position[0] = 10.f;
                               t->position[2] = 5.f;
                           }
                       }); // Add pipeline function work
    p.setInput(g);         // Set pipeline input
    shared_ptr<GameObject> result = static_pointer_cast<GameObject>(p.process());
    shared_ptr<Transform> tresult = static_pointer_cast<Transform>((*result)["Transform"]);
    shared_ptr<Transform> tinput = static_pointer_cast<Transform>((*g)["Transform"]);
    EXPECT_EQ(true, result != NULL);
    EXPECT_EQ(0.f, tinput->position[0]);
    EXPECT_EQ(0.f, tinput->position[2]);
    EXPECT_FLOAT_EQ(10.f, tresult->position[0]);
    EXPECT_FLOAT_EQ(5.f, tresult->position[2]);
}

TEST(PipelineTest, MultipleComponents)
{
    Pipeline p;
    shared_ptr<GameObject> g(new GameObject());
    shared_ptr<Transform> t(new Transform());
    shared_ptr<Rigidbody> r(new Rigidbody());
    shared_ptr<Info> i(new Info("Testing game object"));
    //(*g)["Transform"] = t;
    //(*g)["Rigidbody"] = r;
    //(*g)["Info"] = i;
    g->setComponent("Transform", t);
    g->setComponent("Rigidbody", r);
    g->setComponent("Info", i);

    EXPECT_EQ(3, g->size());
    p.pushPipelineNode("Move", [](shared_ptr<ComponentsContainer> c)
                       {
                           if (c->count("Transform"))
                           {
                               shared_ptr<Transform> t = c->getComponent<Transform>("Transform");
                               t->position[0] = 10.f;
                               t->position[2] = 5.f;
                           }
                           if (c->count("Rigidbody"))
                           {
                               shared_ptr<Rigidbody> r = static_pointer_cast<Rigidbody>((*c)["Rigidbody"]);
                               r->gravity = true;
                               r->mass = 100;
                           }
                           if (c->count("Info"))
                           {
                               shared_ptr<Info> i = static_pointer_cast<Info>((*c)["Info"]);
                               i->setInfo("TEST");
                           }
                       });
    p.setInput(g);
    shared_ptr<GameObject> result = p.process<GameObject>();
    EXPECT_EQ(true, result != NULL);
    shared_ptr<Transform> tresult = static_pointer_cast<Transform>((*result)["Transform"]);
    shared_ptr<Rigidbody> rresult = static_pointer_cast<Rigidbody>((*result)["Rigidbody"]);
    shared_ptr<Info> iresult = static_pointer_cast<Info>((*result)["Info"]);
    EXPECT_FLOAT_EQ(10.f, tresult->position[0]);
    EXPECT_FLOAT_EQ(5.f, tresult->position[2]);
    EXPECT_EQ(true, rresult->gravity);
    EXPECT_EQ(100, rresult->mass);
    EXPECT_EQ("TEST", iresult->getInfo());
}

TEST(PipelineTest, MultipleComponentsMultipleNodes)
{
    Pipeline p;
    shared_ptr<GameObject> g(new GameObject());
    shared_ptr<Transform> t(new Transform());
    shared_ptr<Rigidbody> r(new Rigidbody());
    shared_ptr<Info> i(new Info("Testing game object"));
    (*g)["Transform"] = t;
    (*g)["Rigidbody"] = r;
    (*g)["Info"] = i;
    EXPECT_EQ(3, g->size());
    p.pushPipelineNode("Move", [](shared_ptr<ComponentsContainer> c)
                       {
                           if (c->count("Transform"))
                           {
                               shared_ptr<Transform> t = static_pointer_cast<Transform>((*c)["Transform"]);
                               t->position[0] = 10.f;
                               t->position[2] = 5.f;
                           }
                           if (c->count("Rigidbody"))
                           {
                               shared_ptr<Rigidbody> r = static_pointer_cast<Rigidbody>((*c)["Rigidbody"]);
                               r->gravity = true;
                               r->mass = 100;
                           }
                           if (c->count("Info"))
                           {
                               shared_ptr<Info> i = static_pointer_cast<Info>((*c)["Info"]);
                               i->setInfo("TEST");
                           }
                       });
    p.pushPipelineNode("Grow", [](shared_ptr<ComponentsContainer> c)
                       {
                           if (c->count("Transform"))
                           {
                               shared_ptr<Transform> t = static_pointer_cast<Transform>((*c)["Transform"]);
                               t->scale[0] = 5.f;
                               t->scale[1] = 5.f;
                               t->scale[2] = 5.f;
                           }
                       });
    p.pushPipelineNode("ChangeType", [](shared_ptr<ComponentsContainer> c)
                       {
                           if (c->count("Info"))
                           {
                               shared_ptr<Info> i = static_pointer_cast<Info>((*c)["Info"]);
                               i->setInfo("TEST2");
                           }
                       });
    p.setInput(g);
    shared_ptr<GameObject> result = static_pointer_cast<GameObject>(p.process());
    EXPECT_EQ(true, result != NULL);
    shared_ptr<Transform> tresult = static_pointer_cast<Transform>((*result)["Transform"]);
    shared_ptr<Rigidbody> rresult = static_pointer_cast<Rigidbody>((*result)["Rigidbody"]);
    shared_ptr<Info> iresult = static_pointer_cast<Info>((*result)["Info"]);
    EXPECT_FLOAT_EQ(10.f, tresult->position[0]);
    EXPECT_FLOAT_EQ(5.f, tresult->position[2]);
    EXPECT_FLOAT_EQ(5.f, tresult->scale[0]);
    EXPECT_FLOAT_EQ(5.f, tresult->scale[1]);
    EXPECT_FLOAT_EQ(5.f, tresult->scale[2]);
    EXPECT_EQ(true, rresult->gravity);
    EXPECT_EQ(100, rresult->mass);
    EXPECT_EQ("TEST2", iresult->getInfo());
}

TEST(PipelineTest, AddComponentsThroughNodeFunctions)
{
    Pipeline p;
    shared_ptr<GameObject> g(new GameObject());
    shared_ptr<Transform> t(new Transform());
    shared_ptr<Rigidbody> r(new Rigidbody());
    (*g)["Transform"] = t;
    (*g)["Rigidbody"] = r;
    EXPECT_EQ(2, g->size());
    p.pushPipelineNode("Move", [](shared_ptr<ComponentsContainer> c)
                       {
                           if (c->count("Transform"))
                           {
                               shared_ptr<Transform> t = static_pointer_cast<Transform>((*c)["Transform"]);
                               t->position[0] = 10.f;
                               t->position[2] = 5.f;
                           }
                           if (c->count("Rigidbody"))
                           {
                               shared_ptr<Rigidbody> r = static_pointer_cast<Rigidbody>((*c)["Rigidbody"]);
                               r->gravity = true;
                               r->mass = 100;
                           }
                       });
    p.pushPipelineNode("Grow", [](shared_ptr<ComponentsContainer> c)
                       {
                           if (c->count("Transform"))
                           {
                               shared_ptr<Transform> t = static_pointer_cast<Transform>((*c)["Transform"]);
                               t->scale[0] = 5.f;
                               t->scale[1] = 5.f;
                               t->scale[2] = 5.f;
                           }
                       });
    p.pushPipelineNode("AddInfo", [](shared_ptr<ComponentsContainer> c)
                       {
                           shared_ptr<Info> i(new Info("Testing game object"));
                           i->setInfo("TEST2");
                           (*c)["Info"] = i;
                       });
    p.pushPipelineNode("ChangeType", [](shared_ptr<ComponentsContainer> c)
                       {
                           if (c->count("Info"))
                           {
                               shared_ptr<Info> i = static_pointer_cast<Info>((*c)["Info"]);
                               i->setInfo("TEST2");
                           }
                       });
    p.setInput(g);
    shared_ptr<GameObject> result = static_pointer_cast<GameObject>(p.process());
    EXPECT_EQ(true, result != NULL);
    shared_ptr<Transform> tresult = static_pointer_cast<Transform>((*result)["Transform"]);
    shared_ptr<Rigidbody> rresult = static_pointer_cast<Rigidbody>((*result)["Rigidbody"]);
    shared_ptr<Info> iresult = static_pointer_cast<Info>((*result)["Info"]);
    EXPECT_FLOAT_EQ(10.f, tresult->position[0]);
    EXPECT_FLOAT_EQ(5.f, tresult->position[2]);
    EXPECT_FLOAT_EQ(5.f, tresult->scale[0]);
    EXPECT_FLOAT_EQ(5.f, tresult->scale[1]);
    EXPECT_FLOAT_EQ(5.f, tresult->scale[2]);
    EXPECT_EQ(true, rresult->gravity);
    EXPECT_EQ(100, rresult->mass);
    EXPECT_EQ("TEST2", iresult->getInfo());
}
