#include <iostream>
#include <cstring>
#include <sstream>

#define INT_LENGTH 10

struct Node {
    int* keys;
    Node** subNodes;
    int nodeCount = 0;
    int degree;
    int keysCount = 0;

    explicit Node(int degree) {
        this->degree = degree;
        keys = new int[2 * degree - 1];
        subNodes = new Node* [2 * degree];
    }

    ~Node() {
        delete[] keys;
        delete[] subNodes;
    }
};

void dispose(Node* node) {
    if (node == nullptr) return;
    for (int i = 0; i < node->nodeCount; i++) {
        dispose(node->subNodes[i]);
        delete node->subNodes[i];
    }
    delete node;
}

void split(Node* base, Node* node, int pivot) {
    Node* newNode = new Node(node->degree);

    for (int i = base->keysCount - 1; i >= pivot; i--) {
        base->keys[i + 1] = base->keys[i];
    }
    for (int i = base->keysCount; i >= pivot + 1; i--) {
        base->subNodes[i + 1] = base->subNodes[i];
    }
    newNode->keysCount = node->degree - 1;
    for (int i = 0; i < newNode->keysCount; i++) {
        newNode->keys[i] = node->keys[node->degree + i];
    }
    if (node->nodeCount != 0) {
        for (int i = 0; i < node->degree; i++) {
            newNode->subNodes[i] = node->subNodes[node->degree + i];
            newNode->nodeCount++;
        }
    }
    node->keysCount = node->degree - 1;

    base->subNodes[pivot + 1] = newNode;
    base->nodeCount++;
    base->keys[pivot] = node->keys[node->degree - 1];
    base->keysCount++;
}

Node* addValue(Node* node, int value) {
    if (node->keysCount < node->degree * 2 - 1) {
        if (node->nodeCount > 0) {
            int i = node->keysCount - 1;
            for (; i >= 0 && node->keys[i] > value; i--);
            if (node->subNodes[i + 1]->keysCount == node->degree * 2 - 1) {
                split(node, node->subNodes[i + 1], i + 1);
                i += node->keys[i + 1] < value ? 1 : 0;
            }
            addValue(node->subNodes[i + 1], value);
        } else {
            int i = node->keysCount - 1;
            for (; i >= 0 && node->keys[i] > value; i--) {
                node->keys[i + 1] = node->keys[i];
            }
            node->keys[i + 1] = value;
            node->keysCount++;
        }
        return node;
    }
    Node* newNode = new Node(node->degree);
    newNode->subNodes[0] = node;
    newNode->nodeCount = 1;
    split(newNode, node, 0);
    int n = newNode->keys[0] < value ? 1 : 0;
    addValue(newNode->subNodes[n], value);
    return newNode;
}


bool checkValue(Node* node, int value) {
    int i = 0;
    for (; i < node->keysCount && value > node->keys[i]; i++);
    if (i < node->keysCount && node->keys[i] == value) return true;
    if (node->nodeCount == 0) return false;
    return checkValue(node->subNodes[i], value);
}

void printTree(Node* node) {
    for (int i = 0; i <= node->keysCount; i++) {
        if (node->nodeCount != 0) {
            printTree(node->subNodes[i]);
        }
        if (i != node->keysCount) {
            std::cout << node->keys[i] << " ";
        }
    }
}

struct Stack {
    Node* data;
    Stack* next;
};

Stack* pop(Stack* head, Node** output) {
    *output = head->data;
    Stack* top = head->next;
    delete head;
    return top;
}

Stack* push(Stack* head, Node* value) {
    Stack* node = new Stack;
    node->data = value;
    node->next = head;
    return node;
}

Node* load(int degree) {
    char input[INT_LENGTH];
    Node* current = nullptr;
    Stack* head = nullptr;
    while (std::cin >> input) {
        if (strcmp(input, "(") == 0) {
            if (current != nullptr) {
                head = push(head, current);
            }
            current = new Node(degree);
        } else if (strcmp(input, ")") == 0) {
            if (head == nullptr) {
                return current;
            }
            Node* top;
            head = pop(head, &top);
            top->subNodes[top->nodeCount++] = current;
            current = top;
        } else {
            int number = atoi(input);
            current->keys[current->keysCount++] = number;
        }
    }
    return nullptr;
}

void serialize(Node* node) {
    std::cout << "( ";
    for (int i = 0; i <= node->keysCount; i++) {
        if (node->nodeCount >= i + 1) {
            serialize(node->subNodes[i]);
        }
        if (i != node->keysCount) {
            std::cout << node->keys[i] << " ";
        }
    }
    std::cout << ") ";
}

void join(Node* node, int i) {
    Node* first = node->subNodes[i];
    Node* second = node->subNodes[i + 1];

    first->keys[node->degree - 1] = node->keys[i];
    first->keysCount++;
    for (int j = 0; j < second->keysCount; j++) {
        first->keys[j + node->degree] = second->keys[j];
    }
    first->keysCount += second->keysCount;
    if (first->nodeCount > 0) {
        for (int j = 0; j <= second->keysCount; j++) {
            first->subNodes[j + node->degree] = second->subNodes[j];
        }
    }

    delete second;

    for (int j = i + 1; j < node->keysCount; j++) {
        node->keys[j - 1] = node->keys[j];
    }
    for (int j = i + 2; j <= node->keysCount; j++) {
        node->subNodes[j - 1] = node->subNodes[j];
    }
    node->keysCount--;
}

void moveKeys(Node* node, int i) {
    Node* first = node->subNodes[i];
    if (i != 0 && node->subNodes[i - 1]->keysCount >= node->degree) {
        Node* second = node->subNodes[i - 1];
        for (int j = first->keysCount - 1; j >= 0; j--) {
            first->keys[j + 1] = first->keys[j];
        }
        if (first->nodeCount > 0) {
            for (int j = first->keysCount; j >= 0; j--) {
                first->subNodes[j + 1] = first->subNodes[j];
            }
        }
        first->keys[0] = node->keys[i - 1];
        if (first->nodeCount > 0) {
            first->subNodes[0] = second->subNodes[second->keysCount];
        }
        node->keys[i - 1] = second->keys[second->keysCount - 1];
        first->keysCount++;
        second->keysCount--;
        return;
    }
    if (i != node->keysCount && node->subNodes[i + 1]->keysCount >= node->degree) {
        Node* second = node->subNodes[i + 1];

        first->keys[first->keysCount] = node->keys[i];
        if (first->nodeCount > 0) {
            first->subNodes[first->keysCount + 1] = second->subNodes[0];
        }
        node->keys[i] = second->keys[0];
        for (int j = 1; j < second->keysCount; j++) {
            second->keys[j - 1] = second->keys[j];
        }
        if (second->nodeCount > 0) {
            for (int j = 1; j <= second->keysCount; j++) {
                second->subNodes[j - 1] = second->subNodes[j];
            }
        }

        first->keysCount++;
        second->keysCount--;

        return;
    }
    if (i != node->keysCount) {
        join(node, i);
    } else {
        join(node, i - 1);
    }
}

void remove(Node* node, int value) {
    int i = 0;
    for (; i < node->keysCount && node->keys[i] < value; i++);

    if (i < node->keysCount && node->keys[i] == value) {
        if (node->nodeCount == 0) {
            for (int j = i + 1; j < node->keysCount; ++j) {
                node->keys[j - 1] = node->keys[j];
            }
            node->keysCount--;
        } else {
            if (node->subNodes[i]->keysCount >= node->degree) {
                Node* tmp = node->subNodes[i];
                while (tmp->nodeCount > 0) {
                    tmp = tmp->subNodes[tmp->keysCount];
                }
                node->keys[i] = tmp->keys[tmp->keysCount - 1];
                remove(node->subNodes[i], node->keys[i]);
            } else if (node->subNodes[i + 1]->keysCount >= node->degree) {
                Node* tmp = node->subNodes[i + 1];
                while (tmp->nodeCount > 0) {
                    tmp = tmp->subNodes[0];
                }
                node->keys[i] = tmp->keys[0];
                remove(node->subNodes[i + 1], node->keys[i]);
            } else {
                join(node, i);
                remove(node->subNodes[i], value);
            }
        }
        return;
    }
    if (node->nodeCount == 0) {
        return;
    }
    if (node->subNodes[i]->keysCount < node->degree) {
        moveKeys(node, i);
    }
    if (i > node->keysCount) {
        remove(node->subNodes[i - 1], value);
    } else {
        remove(node->subNodes[i], value);
    }

}

int search(Node* node, int value) {
    int i = 0;
    for (; i < node->keysCount && node->keys[i] < value; i++);
    if (node->keys[i] == value) return 1;
    if (node->nodeCount > 0) return 1 + search(node->subNodes[i], value);
    return 1;
}

void cache(Node* node, int size) {
    int cacheSum = 0;
    int sum = 0;
    int* cache = new int[size];
    for (int i = 0; i < size; i++) {
        cache[i] = 0;
    }
    int value;
    std::string line;
    getline(std::cin, line);
    std::istringstream input(line);
    while (input >> value) {
        int currentSum = search(node, value);
        sum += currentSum;
        bool isCached = false;
        for (int i = 0; i < size; i++) {
            if (cache[i] == value) {
                isCached = true;
                break;
            }
        }
        if (!isCached) {
            for (int i = size - 1; i > 0; i--) {
                cache[i] = cache[i - 1];
            }
            cache[0] = value;
            cacheSum += currentSum;
        }
    }
    std::cout << "NO CACHE: " << sum << " CACHE: " << cacheSum;
    delete[] cache;
}

int main() {
    Node* head = nullptr;
    char input;
    while (std::cin >> input) {
        switch (input) {
            case 'I':
                delete head;
                int degree;
                std::cin >> degree;
                head = new Node(degree);
                break;
            case 'A':
                int value;
                std::cin >> value;
                head = addValue(head, value);
                break;
            case '?':
                int n;
                std::cin >> n;
                std::cout << n << " " << (checkValue(head, n) ? '+' : '-') << "\n";
                break;
            case 'L':
                int i;
                std::cin >> i;
                delete head;
                head = load(i);
                break;
            case 'S':
                std::cout << head->degree << "\n";
                serialize(head);
                break;
            case 'P':
                printTree(head);
                break;
            case 'R':
                int removed;
                std::cin >> removed;
                remove(head, removed);
                break;
            case 'C':
                int size;
                std::cin >> size;
                cache(head, size);
                break;
            case '#':
                break;
            case 'X':
                delete head;
                return 0;
            default:
                std::cout << input << "\n";
                break;
        }
    }
    dispose(head);
    return 0;
}
