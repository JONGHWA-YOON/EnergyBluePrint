#include "YunutyEngine.h"

// Yunuty.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//
//#include <iostream>
//#include "YunutyCycle.h"
//#include "Transform.h"
//#include "RectTransform.h"
//#include "GameObject.h"
//#include "Scene.h"
//#include "Vector.h"
//#include "Vector2.h"
//#include "Vector3.h"
//#include <queue>
//#include <stack>
//
//using namespace std;
//using namespace YunutyEngine;
//int main()
//{
//
//    Scene scene0;
//    scene0.setName("default");
//    GameObject* a = scene0.AddGameObject("a");
//    GameObject* b = scene0.AddGameObject("b");
//    GameObject* c = scene0.AddGameObject("c");
//    GameObject* d = scene0.AddGameObject("d");
//    GameObject* e = scene0.AddGameObject("e");
//    GameObject* a2 = scene0.AddGameObject("a2",a);
//    GameObject* b2 = scene0.AddGameObject("b2",b);
//    GameObject* c2 = scene0.AddGameObject("c2",c);
//    GameObject* c3 = scene0.AddGameObject("c3",c);
//    GameObject* d2 = scene0.AddGameObject("d2",d);
//    GameObject* e2 = scene0.AddGameObject("e2",e);
//
//    c2->SetParent(d2);
//    //auto trs = c->AddComponent<RectTransform>();
//    stack<GameObject*> yunu;
//    yunu.push(e);
//    yunu.push(d);
//    yunu.push(c);
//    yunu.push(b);
//    yunu.push(a);
//
//    while (!yunu.empty())
//    {
//        auto top = yunu.top();
//        yunu.pop();
//        auto children = top->GetChildren();
//        for (auto i = children.rbegin(); i != children.rend(); i++)
//            yunu.push(*i);
//
//        cout<< top->getName()<< " : " << top->GetSceneIndex() << endl;
//    }
//
//    Scene::LoadScene(&scene0);
//
//    YunutyCycle::GetInstance().Initialize();
//    YunutyCycle::GetInstance().Play();
//    while (true);
//    std::cout << "Hello World!\n";
//}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
