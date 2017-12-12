#include "Menu.h"

MenuNode::MenuNode(string new_label, void (*callBackPtr)(), MenuNode *parentPtr, string new_header) {
    label = new_label;
    header = new_header;
    callBack = callBackPtr;
    parent = parentPtr;
}

void MenuNode::add(MenuNode *item) {
    children.push_back(item);
}

string MenuNode::getLabel() {
    return label;
}

string MenuNode::getHeader() {
    return header;
}

bool MenuNode::headerExist() {
    if(header == "") {
        return false;
    }
    return true;
}

void MenuNode::runCallback() {
    callBack();
}

bool MenuNode::callbackExist() {
    if(callBack == NULL) {
        return false;
    }
    return true;
}

int MenuNode::getMenuNodeSize() {
    return children.size();
}

MenuNode *MenuNode::getChild(int index) {
    if(index < children.size())
        return children[index];
    else {
        return false;
    }
}

bool MenuNode::parentExist() {
    if(parent == NULL) {
        return false;
    }
    return true;
}

MenuNode *MenuNode::getParent() {
    return parent;
}

MenuNode::~MenuNode() {
    children.clear();
}
