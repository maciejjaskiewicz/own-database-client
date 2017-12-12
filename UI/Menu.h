#ifndef MENUNODE_H
#define MENUNODE_H

#include <iostream>
#include <vector>

using namespace std;

class MenuNode {

private:
    string label;
    string header;
    void (*callBack)();
    vector<MenuNode *> children;
    MenuNode *parent;

public:
    MenuNode(string new_label, void (*callBackPtr)(), MenuNode *parentPtr = NULL, string new_header = "");
    void add(MenuNode *item);
    string getLabel();
    string getHeader();
    bool headerExist();
    void runCallback();
    bool callbackExist();
    int getMenuNodeSize();
    MenuNode *getChild(int index);
    bool parentExist();
    MenuNode *getParent();
    ~MenuNode();
};

#endif // MENUNODE_H
