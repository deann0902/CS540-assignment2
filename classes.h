#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <stack>

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
    int currPage = 1;
    int SlotDsize = 0;
    string gNewFileName;
    vector<char> Data;
    stack<string> SlotD;

    void StoreRecord() {
        string sizeofData = to_string(Data.size());
        string sizeofentries = to_string(numRecords);

        fstream fin;
        fin.open(gNewFileName,ios::out| ios::in);
        if(fin.is_open()) { 
            fin.seekp(PAGE_SIZE*(currPage - 1));
            for (int i = 0; i < Data.size(); i++) {
                fin << Data[i];
            }
            fin.seekp(PAGE_SIZE*currPage - SlotDsize - sizeofentries.length() - sizeofData.length() - 3);
            //stop string
            fin << "$";
            while(!SlotD.empty()) {
                string kpop = SlotD.top();
                fin << kpop;
                SlotD.pop();
            }
            fin << sizeofentries;
            fin << ",";
            fin << sizeofData;
            fin << "\n";
        }
        fin.close();
        return;
    }


    // Insert new record 
    void insertRecord(Record record) {
        // record.print();
        vector<char> tempData;
        int tempsize;
        string t;
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
        string index = to_string(currentRecordPos) + "," + to_string(tempsize) + ",";
        // Add record to the block

        // Catch SlotDirectory in temp vec
        
        string sizeofData = to_string(Data.size());
        string sizeofentries = to_string(numRecords);
        
        // Our page will be like (Records + "$" sign + SlotDirectory + Number of entries + "," + Pointer to free space + "\n")
        // if the size of new record plus the size of the exists records doesn't exceed the size of a page, we add this record
        if ((tempData.size() + index.length()) + (Data.size() + SlotDsize) + 1 + sizeofentries.length() + 1 + sizeofData.length() + 1 <= PAGE_SIZE) {
            currentRecordPos += tempsize;
            numRecords++;
            for (int i = 0; i < tempsize; i++) {
                Data.push_back(tempData[i]);
            }
            SlotD.push(index);
            SlotDsize += index.length();
            return;
        }
        
        // Take neccessary steps if capacity is reached (you've utilized all the blocks in main memory)
        // Because the page fault, we write our page into the file and flush the page for new records
        else {
            //First we merge the record and slotdirectory
            
            fstream fin;
            fin.open(gNewFileName,ios::out| ios::in);
            if(fin.is_open()) { 
                fin.seekp(PAGE_SIZE*(currPage - 1));
                for (int i = 0; i < Data.size(); i++) {
                    fin << Data[i];
                }
                fin.seekp(PAGE_SIZE*currPage - SlotDsize - sizeofentries.length() - sizeofData.length() - 3);
                //stop string
                fin << "$";
                while(!SlotD.empty()) {
                    string kpop = SlotD.top();
                    fin << kpop;
                    SlotD.pop();
                }
                fin << sizeofentries;
                fin << ",";
                fin << sizeofData;
                fin << "\n";
            }
            fin.close();
            currPage++;

            //clear after write into the file
            SlotDsize = 0;
            numRecords = 0;
            currentRecordPos = 0;
            Data.clear();
            SlotD = {};

        }
        // Then write after clear 
        numRecords++;
        index = to_string(currentRecordPos) + "," + to_string(tempsize) + ",";
        for (int i = 0; i < tempsize; i++) {
            Data.push_back(tempData[i]);
        }
        SlotD.push(index);
        SlotDsize += index.length();

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
            if (lineinfile.empty()) {
                // End of the file, congrat
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
    ifstream fin(gNewFileName, ios::binary);
    if (!fin.is_open()) {
        cerr << "Error: could not open file " << gNewFileName << "\n";
        return Record();
    }

    const int maxPagesPerBatch = 3; 
    int currentBatchStartPage = 0; 
    bool found = false; 
    Record foundRecord; 

    while (!found) {
        for (int i = 0; i < maxPagesPerBatch; ++i) {
            int currentPage = currentBatchStartPage + i;
            fin.seekg(PAGE_SIZE * currentPage, ios::beg);

            vector<char> buffer(PAGE_SIZE);
            fin.read(buffer.data(), PAGE_SIZE);
            if (fin.gcount() == 0) {
                break; 
            }

            
            string endMarker = "$";
            size_t markerPos = string(buffer.begin(), buffer.end()).find(endMarker);
            if (markerPos == string::npos) {
                continue; 
            }

            string slotDirString(buffer.begin() + markerPos + endMarker.length(), buffer.end());
            stringstream slotDirStream(slotDirString);
            string slotValue;
            vector<int> slotData;
            while (getline(slotDirStream, slotValue, ',')) {
                if (!slotValue.empty() && all_of(slotValue.begin(), slotValue.end(), ::isdigit)) {
                    slotData.push_back(stoi(slotValue));
                }  else {
                    cerr << "Invalid slot value: " << slotValue << "\n";
                    break; 
                }
            }

            for (size_t j = 0; j < slotData.size() - 1; j += 2) {
                int recordStart = slotData[j];
                int recordSize = slotData[j + 1];
                if (recordStart + recordSize <= PAGE_SIZE) {
                    string recordData(buffer.begin() + recordStart, buffer.begin() + recordStart + recordSize);
                    stringstream ss(recordData);
                    vector<string> fields;
                    string field;
                    while (getline(ss, field, ',')) {
                        fields.push_back(field);
                    }

                    if (fields.size() >= 4 && stoi(fields[0]) == id) {
                        found = true;
                        foundRecord = Record(fields);
                        break;
                    }
                }
            }

            if (found) {
                break; 
            }
        }

        if (found) {
            break; 
        }

        currentBatchStartPage += maxPagesPerBatch; 
    }

    fin.close();
    if (found) {
        return foundRecord; 
    } else {
        cerr << "Record with ID " << id << " not found\n";
        return Record(); 
    }
}
};
