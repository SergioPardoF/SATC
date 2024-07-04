#ifndef BPlusTree_H
#define BPlusTree_H

#include <iostream>
#include <fstream>

template <typename T1, typename T2>
struct Node {
    bool is_leaf;
    std::size_t degree; // maximum number of children
    std::size_t size; // current number of item
    T1* item;
    T2* value;
    Node<T1,T2>** children;
    Node<T1,T2>* parent;

public:
    Node(std::size_t _degree) {// Constructor
        this->is_leaf = false;
        this->degree = _degree;
        this->size = 0;

        T1* _item = new T1[degree-1];
        for(int i=0; i<degree-1; i++){
            _item[i] = 0;
        }
        this->item = _item;

        T2* _value = new T2[degree-1];
        for(int i=0; i<degree-1; i++){
            _value[i] = 0;
        }
        this->value = _value;

        Node<T1,T2>** _children = new Node<T1,T2>*[degree];
        for(int i=0; i<degree; i++){
            _children[i] = nullptr;
        }
        this->children = _children;

        this->parent = nullptr;

    }
};

template <typename T1, typename T2>
class BPlusTree {
    Node<T1,T2>* root;
    std::size_t degree;

public:
    BPlusTree(std::size_t _degree) {// Constructor
        this->root = nullptr;
        this->degree = _degree;
    }
    ~BPlusTree() { // Destructor
        clear(this->root);
    }

    Node<T1,T2>* getroot(){
        return this->root;
    }

    Node<T1,T2>* BPlusTreeSearch(Node<T1,T2>* node, T1 key){
        if(node == nullptr) { // if root is null, return nullptr
            return nullptr;
        }
        else{
            Node<T1,T2>* cursor = node; // cursor finding key

            while(!cursor->is_leaf){ // until cusor pointer arrive leaf
                for(int i=0; i<cursor->size; i++){ //in this index node, find what we want key
                    if(key < cursor->item[i]){ //find some range, and let find their child also.
                        cursor = cursor->children[i];
                        break;
                    }
                    if(i == (cursor->size)-1){
                        cursor = cursor->children[i+1];
                        break;
                    }
                }
            }

            //search for the key if it exists in leaf node.
            for(int i=0; i<cursor->size; i++){
                if(cursor->item[i] == key){
                    return cursor;
                }
            }

            return nullptr;
        }
    }
    Node<T1,T2>* BPlusTreeRangeSearch(Node<T1,T2>* node, T1 key){
        if(node == nullptr) { // if root is null, return nullptr
            return nullptr;
        }
        else{
            Node<T1,T2>* cursor = node; // cursor finding key
            //int it = 0;
            
            while(!cursor->is_leaf){ // until cusor pointer arrive leaf
                //std::cout<<"-- While it "<<it<<std::endl;
                //it++;
                for(int i=0; i<cursor->size; i++){ //in this index node, find what we want key
                    //std::cout<<"-- For it "<<i<<std::endl;
                    if(key < cursor->item[i]){ //find some range, and let find their child also.
                        //std::cout<<"--Key < item"<<std::endl;
                        cursor = cursor->children[i];
                        break;
                    }
                    //std::cout<<"---- key = "<<key<<" | item = "<<cursor->item[i]<<std::endl;
                    if(i == (cursor->size)-1){
                        //std::cout<<"-- i == cursor size"<<std::endl;
                        cursor = cursor->children[i+1];
                        break;
                    }
                }
                //std::cout<<"-- Exit For"<<std::endl;
            }
            //std::cout<<"-- Exit While"<<std::endl;
            return cursor;
        }
    }

    int range_search(T1 start, T1 end, std::vector<T1>& result_keys, std::vector<T2>& result_values) {
        int index = 0;

        Node<T1, T2>* start_node = BPlusTreeRangeSearch(this->root, start);
        Node<T1, T2>* cursor = start_node;
        T1 temp = cursor->item[0];

        while (temp <= end && cursor != nullptr) {
            for (int i = 0; i < cursor->size; i++) {
                temp = cursor->item[i];
                if ((temp >= start) && (temp <= end)) {
                    result_keys.push_back(temp);
                    result_values.push_back(cursor->value[i]);
                    index++;
                }
            }
            if (temp <= end) {
                cursor = cursor->children[cursor->size];
                temp = cursor ? cursor->item[0] : end + 1;
            }
        }
        return index;
    }

    bool search(T1 data) {  // Return true if the item exists. Return false if it does not.
        return BPlusTreeSearch(this->root, data) != nullptr;
    }

    // Given an array arr of ordered len elements of type T1, return the index where data should be inserted
    int find_index(T1* arr, T1 data, int len){
        int index = 0;
        for(int i=0; i<len; i++){
            if(data < arr[i]){
                index = i;
                break;
            }
            if(i==len-1){
                index = len;
                break;
            }
        }
        return index;
    }

    T1* item_insert(T1* arr, T1 data, T2 value, T2* values, int len){
        int index = 0;
        for(int i=0; i<len; i++){
            if(data < arr[i]){
                index = i;
                break;
            }
            if(i==len-1){
                index = len;
                break;
            }
        }

        for(int i = len; i > index; i--){
            arr[i] = arr[i-1];
            values[i] = values[i-1];
        }

        arr[index] = data;
        values[index] = value;

        return arr;
    }

    void pair_insert(T1* &arr, T1 data, T2 value, T2* &values, int len){
        int index = 0;
        for(int i=0; i<len; i++){
            if(data < arr[i]){
                index = i;
                break;
            }
            if(i==len-1){
                index = len;
                break;
            }
        }

        for(int i = len; i > index; i--){
            arr[i] = arr[i-1];
            values[i] = values[i-1];
        }

        arr[index] = data;
        values[index] = value;
    }

    Node<T1,T2>** child_insert(Node<T1,T2>** child_arr, Node<T1,T2>*child,int len,int index){
        for(int i= len; i > index; i--){
            child_arr[i] = child_arr[i - 1];
        }
        child_arr[index] = child;
        return child_arr;
    }
    Node<T1,T2>* child_item_insert(Node<T1,T2>* node, T1 data, T2 value, Node<T1,T2>* child){
        int item_index=0;
        int child_index=0;
        for(int i=0; i< node->size; i++){
            if(data < node->item[i]){
                item_index = i;
                child_index = i+1;
                break;
            }
            if(i==node->size-1){
                item_index = node->size;
                child_index = node->size+1;
                break;
            }
        }
        for(int i = node->size;i > item_index; i--){
            node->item[i] = node->item[i-1];
            node->value[i] = node->value[i-1];
        }
        for(int i=node->size+1;i>child_index;i--){
            node->children[i] = node->children[i-1];
        }

        node->item[item_index] = data;
        node->value[item_index] = value;
        node->children[child_index] = child;

        return node;
    }
    void InsertPar(Node<T1,T2>* par,Node<T1,T2>* child, T1 data, T2 value){
        //std::cout<<"Entering InsertPar"<<std::endl;
        //overflow check
        Node<T1,T2>* cursor = par;
        if(cursor->size < this->degree-1){//not overflow, just insert in the correct position
            //insert item, child, and reallocate
            //std::cout<<"Insertion without overflow (just rearrange)"<<std::endl;
            cursor = child_item_insert(cursor,data,value,child);
            cursor->size++;
        }
        else{//overflow
            //make new node
            //std::cout<<"Insertion on overflow case"<<std::endl;
            auto* Newnode = new Node<T1,T2>(this->degree);
            Newnode->parent = cursor->parent;

            //copy item
            T1* item_copy = new T1[cursor->size+1];
            for(int i=0; i<cursor->size; i++){
                item_copy[i] = cursor->item[i];
            }
            item_copy = item_insert(item_copy,data, value, cursor->value, cursor->size);

            auto** child_copy = new Node<T1,T2>*[cursor->size+2];
            for(int i=0; i<cursor->size+1;i++){
                child_copy[i] = cursor->children[i];
            }
            child_copy[cursor->size+1] = nullptr;
            child_copy = child_insert(child_copy,child,cursor->size+1,find_index(item_copy,data,cursor->size+1));

            //split nodes
            cursor->size = (this->degree)/2;
            if((this->degree) % 2 == 0){
                Newnode->size = (this->degree) / 2 -1;
            }
            else{
                Newnode->size = (this->degree) / 2;
            }

            for(int i=0; i<cursor->size;i++){
                cursor->item[i] = item_copy[i];
                cursor->children[i] = child_copy[i];
            }
            cursor->children[cursor->size] = child_copy[cursor->size];
            //todo 안지워짐. 뒤에것.

            for(int i=0; i < Newnode->size; i++){
                Newnode->item[i] = item_copy[cursor->size + i +1];
                Newnode->children[i] = child_copy[cursor->size+i+1];
                Newnode->children[i]->parent=Newnode;
            }
            Newnode->children[Newnode->size] = child_copy[cursor->size+Newnode->size+1];
            Newnode->children[Newnode->size]->parent=Newnode;

            T1 paritem = item_copy[this->degree/2];

            delete[] item_copy;
            delete[] child_copy;

            //parent check
            if(cursor->parent == nullptr){//if there are no parent node(root case)
                auto* Newparent = new Node<T1,T2>(this->degree);
                cursor->parent = Newparent;
                Newnode->parent = Newparent;

                Newparent->item[0] = paritem;
                Newparent->size++;

                Newparent->children[0] = cursor;
                Newparent->children[1] = Newnode;

                this->root = Newparent;

                //delete Newparent;
            }
            else{//if there already have parent node
                InsertPar(cursor->parent, Newnode, paritem, value);
            }
        }
    }

    void insert(T1 data, T2 value) {
        if(this->root == nullptr){ //if the tree is empty
            //std::cout<<"First insertion"<<std::endl;
            this->root = new Node<T1,T2>(this->degree);
            this->root->is_leaf = true;
            this->root->item[0] = data;
            this->root->value[0] = value;
            this->root->size = 1; //
        }
        else{ //if the tree has at least one node
            Node<T1,T2>* cursor = this->root;

            //move to leaf node
            //std::cout<<"Entering first BPlusTreeRangeSearch"<<std::endl;
            cursor = BPlusTreeRangeSearch(cursor, data);


            //overflow check
            if(cursor->size < (this->degree-1)){ // not overflow, just insert in the correct position
                //std::cout<<"Insertion without overflow (just rearrange)"<<std::endl;
                //item insert and rearrange
                //cursor->item = item_insert(cursor->item,data, value, cursor->value, cursor->size);
                pair_insert(cursor->item,data, value, cursor->value, cursor->size);
                cursor->size++;
                //edit pointer(next node)
                cursor->children[cursor->size] = cursor->children[cursor->size-1];
                cursor->children[cursor->size-1] = nullptr;
            }
            else{//overflow case
                //make new node
                //std::cout<<"Insertion on overflow case"<<std::endl;
                auto* Newnode = new Node<T1,T2>(this->degree);
                Newnode->is_leaf = true;
                Newnode->parent = cursor->parent;

                //copy item
                T1* item_copy = new T1[cursor->size+1];
                T2* value_copy = new T2[cursor->size+1];
                for(int i=0; i<cursor->size; i++){
                    item_copy[i] = cursor->item[i];
                    value_copy[i] = cursor->value[i];
                }

                //insert and rearrange
                //item_copy = item_insert(item_copy,data, value, cursor->value, cursor->size);
                pair_insert(item_copy,data, value, value_copy, cursor->size);

                //split nodes
                cursor->size = (this->degree)/2;
                if((this->degree) % 2 == 0){
                    Newnode->size = (this->degree) / 2;
                }
                else{
                    Newnode->size = (this->degree) / 2 + 1;
                }

                for(int i=0; i<cursor->size;i++){
                    cursor->item[i] = item_copy[i];
                    cursor->value[i] = value_copy[i];
                }
                for(int i=0; i < Newnode->size; i++){
                    Newnode->item[i] = item_copy[cursor->size + i];
                    Newnode->value[i] = value_copy[cursor->size +i];
                }

                cursor->children[cursor->size] = Newnode;
                Newnode->children[Newnode->size] = cursor->children[this->degree-1];
                cursor->children[this->degree-1] = nullptr;

                delete[] item_copy;
                delete[] value_copy;

                //parent check
                T1 paritem = Newnode->item[0];

                if(cursor->parent == nullptr){//if there are no parent node(root case)
                    //std::cout<<"Root case"<<std::endl;
                    auto* Newparent = new Node<T1,T2>(this->degree);
                    cursor->parent = Newparent;
                    Newnode->parent = Newparent;

                    Newparent->item[0] = paritem;
                    Newparent->size++;

                    Newparent->children[0] = cursor;
                    Newparent->children[1] = Newnode;

                    this->root = Newparent;
                }
                else{//if there already have parent node
                    //std::cout<<"Parent node exists"<<std::endl;
                    InsertPar(cursor->parent, Newnode, paritem, value);
                }
            }
        }
    }

    // Function to write the B-tree to a binary file
    void writeToFile(const std::string& filename) {
        std::ofstream outFile(filename, std::ios::binary);

        if (!outFile.is_open()) {
            std::cerr << "Failed to open the file for writing." << std::endl;
            return;
        }
        writeBPlusTreeToFile(root, outFile);

        outFile.close();
    }

    // Function to read the B+ tree from a binary file
    void readFromFile(const std::string& filename) {
        std::ifstream inFile(filename, std::ios::binary);

        if (!inFile.is_open()) {
            std::cerr << "Failed to open the file for reading." << std::endl;
            return;
        }

        Node<T1, T2>* last_leaf = nullptr;
        root = readBPlusTreeFromFile(inFile, &last_leaf);

        if (root == nullptr){
            std::cout << "Something went wrong" << std::endl;
        }
        inFile.close();
    }

private:
    // Function to write the B+ tree to a binary file
    void writeBPlusTreeToFile(Node<T1, T2>* root, std::ofstream& outFile) {
        if (root == nullptr){
            sdsl::write_member(true, outFile);
            return;
        } else {
            sdsl::write_member(false, outFile);
        }

        // Write node data to the file
        sdsl::write_member(root->is_leaf, outFile);
        sdsl::write_member(root->degree, outFile);
        sdsl::write_member(root->size, outFile);

        sdsl::int_vector<> sdslIntVectorKey(root->size);
        sdsl::int_vector<> sdslIntVectorValue(root->size);
        for (int i = 0; i < root->size; ++i) {
            sdslIntVectorKey[i] = root->item[i];
            sdslIntVectorValue[i] = root->value[i];
        }

        sdsl::util::bit_compress(sdslIntVectorKey);
        sdsl::util::bit_compress(sdslIntVectorValue);
        sdslIntVectorKey.serialize(outFile);
        if (root->is_leaf) {
            sdslIntVectorValue.serialize(outFile);
        }

        // Recursively write child nodes
        if (!root->is_leaf){
            for (int i = 0; i < root->degree; i++) {
                writeBPlusTreeToFile(root->children[i], outFile);
            }
        }
    }

    // Function to read the B+ tree from a binary file
    Node<T1, T2>* readBPlusTreeFromFile(std::ifstream& inFile, Node<T1, T2> **last_leaf) {
        bool is_leaf, nodenull;
        std::size_t degree, size;

        // Check if we have reached the end of the file
        if (inFile.peek() == EOF) {
            // End of the file reached
            return nullptr;
        }
        sdsl::read_member(nodenull, inFile);
        if (nodenull) {
            return nullptr;
        }
        sdsl::read_member(is_leaf, inFile);
        sdsl::read_member(degree, inFile);
        sdsl::read_member(size, inFile);

        Node<T1, T2>* node = new Node<T1, T2>(degree);
        node->is_leaf = is_leaf;
        node->size = size;

        sdsl::int_vector<> sdslKeys(size);
        sdsl::int_vector<> sdslValues(size);
        sdslKeys.load(inFile);
        if (is_leaf){
            sdslValues.load(inFile);
        }

        for (int i = 0; i < size; i++) {
            node->item[i] = sdslKeys[i];
            //std::cout << "key: " << node->item[i] << ", ";
            if (is_leaf) {
                node->value[i] = sdslValues[i];
                //std::cout << "value: " << node->value[i] << " | ";
            }
            //std::cout << std::endl;
        }

        if (!is_leaf){
            for (std::size_t i = 0; i < degree; i++) {
                node->children[i] = readBPlusTreeFromFile(inFile, last_leaf);
                if (node->children[i]) {
                    node->children[i]->parent = node;
                }
            }
        } else {
            if (*last_leaf != nullptr) {
                (*last_leaf)->children[(*last_leaf)->size] = node;
            }
            *last_leaf = node;
        }
        return node;
    }

    bool compare_nodes (Node<T1, T2> *node1, Node<T1, T2> *node2){
        if (node1 == nullptr) {
            if (node2 == nullptr) {
                return true;
            }
            std::cout << "1st false on nullptr if" << std::endl;
            return false;
        } else { // node1 != nullptr
            if (node2 == nullptr){
                std::cout << "2nd false on nullptr if" << std::endl;
                return false;
            }
        }
        // Node info
        //std::cout << "Node1 isleaf: " << node1->is_leaf << " | Node2 isleaf: " << node2->is_leaf << std::endl;
        //std::cout << "Node1 degree: " << node1->degree << " | Node2 degree: " << node2->degree << std::endl;
        //std::cout << "Node1 size: " << node1->size << " | Node2 size: " << node2->size << std::endl;
        if (!(node1->is_leaf == node2->is_leaf) || !(node1->degree == node2->degree) || !(node1->size == node2->size)) {
            std::cout << "Node info different" << std::endl;
            return false;
        }
        // Keys and values
        for (int i = 0; i < node1->size; i++){
            //std::cout << "Node1 key: " << node1->item[i] << " | Node2 key: " << node2->item[i] << std::endl;
            if (!(node1->item[i] == node2->item[i])){
                std::cout << "Keys different" << std::endl;
                return false;
            }
            if (node1->is_leaf) {
                //std::cout << "Node1 value: " << node1->value[i] << " | Node2 value: " << node2->value[i] << std::endl;
                if(!(node1->value[i] == node2->value[i])){
                    std::cout << "Values different on leaf node" << std::endl;
                    return false;
                }
            }
        }
        // Children
        if (!node1->is_leaf){
            for(int i = 0; i < node1->degree; i++){
                if (!compare_nodes(node1->children[i], node2->children[i])) {
                    return false;
                }
            }
        }
        return true;
    }

public:
    void clear(Node<T1,T2>* cursor){
        if(cursor != nullptr){
            if(!cursor->is_leaf){
                for(int i=0; i <= cursor->size; i++){
                    clear(cursor->children[i]);
                }
            }
            delete[] cursor->item;
            delete[] cursor->value;
            delete[] cursor->children;
            delete cursor;
        }
    }
    void bpt_print(){
        print(this->root);
    }
    void print(Node<T1,T2>* cursor) {
        // You must NOT edit this function.
        if (cursor != NULL) {
            for (int i = 0; i < cursor->size; ++i) {
                std::cout << cursor->item[i] << " ";
                if (cursor->is_leaf)
                    std::cout << "("<< cursor->value[i] << ") ";
            }
            std::cout << "\n";

            if (!cursor->is_leaf) {
                for (int i = 0; i < cursor->size + 1; ++i) {
                    print(cursor->children[i]);
                }
            }
        }
    }

    bool equals(BPlusTree<T1, T2> *tree){
        return compare_nodes(this->root, tree->root);
    }

};

#endif
