// Assignment 2: AVL Tree with rotations (C++17)
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

struct AvlNode {
    int key;
    int height;  // height of the subtree rooted here (leaf = 1)
    AvlNode* left;
    AvlNode* right;
    explicit AvlNode(int k) : key(k), height(1), left(nullptr), right(nullptr) {}
};

class AvlTree {
public:
    AvlTree() : root_(nullptr) {}
    ~AvlTree() { destroy(root_); }
    AvlTree(const AvlTree&) = delete;
    AvlTree& operator=(const AvlTree&) = delete;

    void insert(int key, bool verbose = true) {
        verbose_ = verbose;
        root_ = insert(root_, key);
    }
    bool search(int key) const {
        const AvlNode* cur = root_;
        while (cur) {
            if (key == cur->key) return true;
            cur = key < cur->key ? cur->left : cur->right;
        }
        return false;
    }
    int height() const { return h(root_); }
    void inorder() const { inorder(root_); std::cout << '\n'; }
    void printSideways() const { printSideways(root_, 0); }
    // Self-check: BST order, stored heights correct, every balance factor in {-1,0,1}.
    bool isValidAvl() const { return checkRange(root_, nullptr, nullptr); }

private:
    AvlNode* root_;
    bool verbose_ = true;

    static void destroy(AvlNode* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
    static int h(const AvlNode* n) { return n ? n->height : 0; }
    static int balance(const AvlNode* n) { return n ? h(n->left) - h(n->right) : 0; }
    static void update(AvlNode* n) { n->height = 1 + std::max(h(n->left), h(n->right)); }

    // Right rotation: y's left child x becomes the subtree root; x's old right
    // subtree B becomes y's new left subtree.
    static AvlNode* rotateRight(AvlNode* y) {
        AvlNode* x = y->left;
        y->left = x->right;
        x->right = y;
        update(y);
        update(x);
        return x;
    }
    // Left rotation (mirror image): x's right child y becomes the subtree root;
    // y's old left subtree B becomes x's new right subtree.
    static AvlNode* rotateLeft(AvlNode* x) {
        AvlNode* y = x->right;
        x->right = y->left;
        y->left = x;
        update(x);
        update(y);
        return y;
    }

    AvlNode* insert(AvlNode* n, int key) {
        if (!n) return new AvlNode(key);
        if (key < n->key) n->left = insert(n->left, key);
        else if (key > n->key) n->right = insert(n->right, key);
        else return n;  // duplicate ignored

        update(n);
        int bf = balance(n);

        if (bf > 1 && key < n->left->key) {
            say("LL (Left-Left) case at node " + std::to_string(n->key) + ": single RIGHT rotation");
            return rotateRight(n);
        }
        if (bf < -1 && key > n->right->key) {
            say("RR (Right-Right) case at node " + std::to_string(n->key) + ": single LEFT rotation");
            return rotateLeft(n);
        }
        if (bf > 1 && key > n->left->key) {
            say("LR (Left-Right) case at node " + std::to_string(n->key) +
                ": LEFT rotation on " + std::to_string(n->left->key) +
                ", then RIGHT rotation on " + std::to_string(n->key));
            n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        if (bf < -1 && key < n->right->key) {
            say("RL (Right-Left) case at node " + std::to_string(n->key) +
                ": RIGHT rotation on " + std::to_string(n->right->key) +
                ", then LEFT rotation on " + std::to_string(n->key));
            n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }

    void say(const std::string& s) const {
        if (verbose_) std::cout << "    [rebalance] " << s << '\n';
    }

    static void inorder(const AvlNode* n) {
        if (!n) return;
        inorder(n->left);
        std::cout << n->key << ' ';
        inorder(n->right);
    }

    // Prints the tree rotated 90 degrees (root on the left, right subtree on top),
    // each node annotated with (height, balance factor).
    static void printSideways(const AvlNode* n, int depth) {
        if (!n) return;
        printSideways(n->right, depth + 1);
        std::cout << std::string(depth * 6, ' ') << n->key
                  << " (h=" << n->height << ", bf=" << balance(n) << ")\n";
        printSideways(n->left, depth + 1);
    }

    static bool checkRange(const AvlNode* n, const int* lo, const int* hi) {
        if (!n) return true;
        if (lo && n->key <= *lo) return false;
        if (hi && n->key >= *hi) return false;
        if (n->height != 1 + std::max(h(n->left), h(n->right))) return false;
        if (std::abs(balance(n)) > 1) return false;
        return checkRange(n->left, lo, &n->key) && checkRange(n->right, &n->key, hi);
    }
};

// Ordinary (unbalanced) BST used only for the height comparison.
struct PlainNode {
    int key;
    PlainNode* left = nullptr;
    PlainNode* right = nullptr;
    explicit PlainNode(int k) : key(k) {}
};
class PlainBst {
public:
    ~PlainBst() { destroy(root_); }
    void insert(int key) {
        PlainNode** cur = &root_;
        while (*cur) {
            if (key == (*cur)->key) return;
            cur = key < (*cur)->key ? &(*cur)->left : &(*cur)->right;
        }
        *cur = new PlainNode(key);
    }
    int height() const { return height(root_); }
private:
    PlainNode* root_ = nullptr;
    static int height(const PlainNode* n) {
        return n ? 1 + std::max(height(n->left), height(n->right)) : 0;
    }
    static void destroy(PlainNode* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
};

static void demo(const std::string& title, const std::vector<int>& seq) {
    std::cout << "\n--- " << title << " ---\n  insert order:";
    for (int k : seq) std::cout << ' ' << k;
    std::cout << '\n';
    AvlTree t;
    for (int k : seq) {
        std::cout << "  insert " << k << '\n';
        t.insert(k);
    }
    std::cout << "  resulting tree:\n";
    t.printSideways();
    std::cout << "  inorder: ";
    t.inorder();
    std::cout << "  valid AVL: " << (t.isValidAvl() ? "yes" : "NO") << '\n';
}

static void compare(const std::string& title, const std::vector<int>& seq) {
    AvlTree avl;
    PlainBst bst;
    for (int k : seq) { avl.insert(k, false); bst.insert(k); }
    std::cout << "\n--- " << title << " (" << seq.size() << " keys) ---\n  insert order:";
    for (int k : seq) std::cout << ' ' << k;
    std::cout << "\n  AVL inorder: ";
    avl.inorder();
    std::cout << "  AVL height = " << avl.height() << "   valid AVL: "
              << (avl.isValidAvl() ? "yes" : "NO") << '\n';
    std::cout << "  Plain BST height = " << bst.height() << '\n';
    std::cout << "  Final AVL shape:\n";
    avl.printSideways();
}

int main() {
    std::cout << "===== Part A: the four rotation patterns =====";
    demo("LL case (right rotation)", {30, 20, 10});
    demo("RR case (left rotation)", {10, 20, 30});
    demo("LR case (left-right double rotation)", {30, 10, 20});
    demo("RL case (right-left double rotation)", {10, 30, 20});

    std::cout << "\n===== Part B: a longer sequence showing all cases mixed =====";
    demo("Mixed sequence", {50, 20, 60, 10, 30, 25, 5, 4, 70, 65});

    std::cout << "\n\n===== Part C: AVL vs ordinary BST, same insertion order =====";
    compare("Ascending keys (worst case for a plain BST)",
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    compare("Nearly sorted keys",
            {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 12, 18, 22, 28, 33, 3, 1});
    return 0;
}
