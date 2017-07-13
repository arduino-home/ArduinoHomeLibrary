#ifndef __ARDUINO_HOME_LIST_H__
#define __ARDUINO_HOME_LIST_H__

template<typename Node>
class LinkedListIterator {
  Node *current;

public:
  LinkedListIterator(Node *pcur)
   : current(pcur) {
  }

  LinkedListIterator<Node> &operator++() {
    current = current->next;
  }

  Node *operator*() const {
    return current;
  }

  bool operator == (const LinkedListIterator<Node> &other) const {
    return current == other.current;
  }

  bool operator != (const LinkedListIterator<Node> &other) const {
    return !(*this == other);
  }
};

template<typename Node>
class LinkedList {
  Node *head;
  Node *tail;

public:
  explicit LinkedList()
   : head(nullptr), tail(nullptr) {
  }

  void add(Node *node) {
    node->next = nullptr;
    if(head) {
      tail->next = node;
      tail = node;
      return;
    }
    
    head = tail = node;
  }

  LinkedListIterator<Node> begin() {
    return LinkedListIterator<Node>(head);
  }

  LinkedListIterator<const Node> begin() const {
    return LinkedListIterator<const Node>(head);
  }

  LinkedListIterator<Node> end() {
    return LinkedListIterator<Node>(nullptr);
  }

  LinkedListIterator<const Node> end() const {
    return LinkedListIterator<const Node>(nullptr);
  }
};

#endif // __ARDUINO_HOME_LIST_H__
