#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};


class StorageBufferManager {

private:
    
    const int BLOCK_SIZE = 3; // initialize the  block size allowed in main memory according to the question 

    // You may declare variables based on your need 
    const int PAGE_SIZE = 4096;
    int numRecords = 0;
    int currentRecordPos = 0;
    string gNewFileName;
    vector<char> Data;
    vector<char> SlotD;

    void StoreRecord() {
        fstream fin;
        fin.open(gNewFileName,ios::out);
        if(fin.is_open()) { 
            for (int i = 0; i < Data.size(); i++) {
                fin << Data[i];
            }
        }
    }

    // Insert new record 
    void insertRecord(Record record) {
        record.print();
        vector<char> tempData;
        int tempsize;
        string t;
        vector<char> tempSlotD;
        t = to_string(record.id);
        // Add record into temp vec
        char const* id_char = t.c_str();
        for (int i = 0; i < t.length();i++) {
            tempData.push_back(id_char[i]);
        }
        tempData.push_back(',');
        char const* name_char = record.name.c_str();
        for (int i = 0; i < record.name.length();i++) {
            tempData.push_back(name_char[i]);
        }
        tempData.push_back(',');
        char const* bio_char = record.bio.c_str();
        for (int i = 0; i < record.bio.length();i++) {
            tempData.push_back(bio_char[i]);
        }
        t = to_string(record.manager_id);
        tempData.push_back(',');
        char const* manid_char = t.c_str(); 
        for (int i = 0; i < t.length();i++) {
            tempData.push_back(manid_char[i]);
        }
        tempData.push_back(',');

        // Catch record size
        tempsize = tempData.size();
        
        // No records written yet
        if (numRecords == 0) {
            // Initialize first block

            // In first block, we don't consider, just push
            for (int i = 0; i < tempsize; i++) {
                Data.push_back(tempData[i]);
            }
            t = to_string(tempsize);
            char const* DataSize_char = t.c_str();
            SlotD.push_back('0');
            SlotD.push_back(',');
            for (int i = 0; i < t.length();i++) {
                SlotD.push_back(DataSize_char[i]);
            }
            SlotD.push_back(',');
            currentRecordPos += tempsize;
            numRecords++;
            cout<<"\nCurrent Data Size is "<<Data.size()<<"\n";
            return ;
        }
        // Add record to the block

        // Catch SlotDirectory in temp vec
        t = to_string(tempsize);
        char const* DataSize_char = t.c_str();
        string r = to_string(currentRecordPos);
        char const* Pos_char = r.c_str();
        for (int i = 0; i < r.length();i++) {
            tempSlotD.push_back(Pos_char[i]);
        }
        tempSlotD.push_back(',');
        for (int i = 0; i < t.length();i++) {
            tempSlotD.push_back(DataSize_char[i]);
        }
        tempSlotD.push_back(',');
        currentRecordPos += tempsize;
        numRecords++;
        string sizeofData = to_string(Data.size());
        string sizeofentries = to_string(numRecords);
        
        // Our page will be like (Records + SlotDirectory + Number of entries + Pointer to free space)
        // if the size of new record plus the size of the exists records doesn't exceed the size of a page, we add this record
        if ((tempData.size() + tempSlotD.size()) + (Data.size() + SlotD.size()) + sizeofentries.length() + sizeofData.length() <= PAGE_SIZE) {
            for (int i = 0; i < tempsize; i++) {
                Data.push_back(tempData[i]);
            }
            for (int i = 0; i < tempSlotD.size(); i++) {
                SlotD.push_back(tempSlotD[i]);
            }
        }
        
        // Take neccessary steps if capacity is reached (you've utilized all the blocks in main memory)
        // Because the page fault, we write our page into the file and flush the page for new records
        else {
            fstream fin;
            fin.open(gNewFileName,ios::out);
            if(fin.is_open()) { 
                for (int i = 0; i < Data.size(); i++) {
                    fin << Data[i];
                }
                fin << "\n";
            }

            //clear after print
            Data.clear();
            SlotD.clear();

        }
        cout<<"\nCurrent Data Size is "<<Data.size()<<"\n";

    }

public:
    StorageBufferManager(string NewFileName) {
        
        //initialize your variables
        ofstream fin;

        // Create your EmployeeRelation file 
        fin.open(NewFileName,ios::out);
        if(!fin) { 
            cout<<"Error in creating file!!!\n"; 
            return ; 
        } 
        fin.close();
        gNewFileName = NewFileName;

        return;
        
    }

    // Read csv file (Employee.csv) and add records to the (EmployeeRelation)
    void createFromFile(string csvFName) {
        fstream fin;
        fin.open(csvFName,ios::in);
        if (!fin.is_open()) {
            cerr << "Couldn't read file: " << csvFName << "\n";
            return ;
        }
        
        string lineinfile;
        bool Flag = true;
        while (getline(fin,lineinfile) || Flag) {
            if (lineinfile == "") {
                Flag = false;
                StoreRecord();
                break;
            }
            string sttrinfile;
            vector<string> attrvec;
            stringstream stringline(lineinfile);
            while(getline(stringline,sttrinfile,',')) {
                attrvec.push_back(sttrinfile);
            }
            Record somerecord(attrvec);
            insertRecord(somerecord);
            // break;

        }
        fin.close();

        

    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }
};
