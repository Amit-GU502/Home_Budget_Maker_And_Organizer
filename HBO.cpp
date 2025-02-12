#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <string>
using namespace std;
class uData{
    private:
        int Needs,Wants,Savings;
        float Income;
        int sno,dsno,qty,dateD,dateM,dateY;                                       //integers used for writing and reading in file
        string name,date,item_name,category,entry,line;                  //strings used for writing and reading in file
        float price,tprice,ntotal,wtotal,stotal,relative_stotal;                                     //floats used for writing and reading in file
        int addAtLine,skipTillLine,currentLine;                 
        
        /*integers used for writing line in-between file, 
        put value from searchDate()+1 in skipTillLine, 
        use searchMaxSno or searchSno to get AddAtLine value, 
        use both of them in pre and post getBetweenLines()*/
        
        vector<string> lines;                                   //vector used for adding, editing and deleting entry in file
        char choice;                                            //asks if user already have an account or not
        bool found_sno,valid_D,found_D,for_add;
        fstream uFile;

        void setPlanAndIncome(float I,int N=50,int W=30,int S=20){          //Sets Income and Budget Plan when a user is new
            uFile.open(name,ios::out);
            entry="Income: "+to_string(I);
            uFile<<entry<<endl;
            entry="Plan: "+to_string(N)+" "+to_string(W)+" "+to_string(S);
            uFile<<entry<<endl;
            uFile.close();
        }

        void getPlanAndIncome(){        //Gets Income and Budget Plan and stores in Income,Needs,Wants and Savings variables
            string word;
            uFile.open(name,ios::in);
            for(int i=0;i<6;i++){           //test if it's reading new line character or not
                uFile>>word;
                if(i==1){
                    Income=stof(word);
                }
                else if(i==3){
                    Needs=stoi(word);
                }
                else if(i==4){
                    Wants=stoi(word);
                }
                else if(i==5){
                    Savings=stoi(word);
                }
                else
                    continue;
            }
            uFile.close();
        }

        void getDate(){             //A simple Get Date function
            valid_D=true;
            date.clear();
            cout<<"Enter date in DDMMYYYY format seperated by spaces : ";
            cin>>dateD>>dateM>>dateY;
            if(isValidDate(dateD,dateM,dateY)){
                valid_D=true;
                date=to_string(dateD)+" "+to_string(dateM)+" "+to_string(dateY);
            }
            else{
                cout<<"Invalid Date"<<endl;
                valid_D=false;
                }
        }

        bool isLeapYear(int year) {
            return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        }

        bool isValidDate(int day, int month, int year) {
            if (year < 0 || year > 9999) {              // Check if the year is within a valid range
            return false;
            }
            if (month < 1 || month > 12) {              // Check if the month is within a valid range
                return false;
            }
            int maxDays = 31;                           // Check the maximum number of days for each month
            if (month == 4 || month == 6 || month == 9 || month == 11) {
                maxDays = 30;
            } else if (month == 2) {
                maxDays = isLeapYear(year) ? 29 : 28;
            }
            if (day < 1 || day > maxDays) {             // Check if the day is within a valid range
                return false;
            }
            return true;                                // The date is valid
        }

        int searchDate(string Date){    //searches the date and return the line number, if no date is found writes the date entered
            int line_no=0;
            fstream sdFile;
            sdFile.open(name,ios::in);
            if(sdFile.is_open()){
                while(getline(sdFile,line)){
                    if(line=="Date: "+Date){
                        sdFile.close();
                        found_D=true;           //found_D used in display function
                        return line_no;
                    }
                    line_no++;
                }
                found_D=false;
                if(for_add==true){              //toggle for_add in display function to not add unneccessary dates
                    sdFile.close();
                    sdFile.open(name,ios::app);
                    if(sdFile.is_open()){
                        sdFile<<"Date: "+Date<<endl; 
                        sdFile.close();
                        found_D=true;
                        return line_no;
                    }
                }
            }
            else{
                cout<<"Something went wrong"<<endl;
                return -1;
            }
        }

        int searchMaxSno(int skip_Till_Line){        //only use after searchDate is called
            ifstream smsFile(name);
            if (!smsFile) {
                cout<<"Failed to open the file."<<endl;
                return 1;
            }
            currentLine=0;
            addAtLine=skip_Till_Line;
            sno=0;
            while(getline(smsFile,line)){
                if(currentLine<skip_Till_Line){
                    currentLine++;
                    continue;
                }
                else if(line.substr(0,line.find(':'))=="Date"){
                    //addAtLine-=1;
                    break;
                }
                else{
                    sno=stoi(line.substr(0,line.find(' ')));
                    addAtLine++;
                }
            }
            smsFile.close();
            int addAtpLine=addAtLine-1;
            return addAtpLine;
        }

        int searchSno(int d_sno,int skip_Till_Line){       //searches the asked S.No., To be used in edit function. use only after searchDate function is called
            found_sno=true;
            ifstream dsFile(name);
            if (!dsFile) {
                cout<<"Failed to open the file."<<endl;
                found_sno=false;
                return -1;
            }
            currentLine=0;
            addAtLine=skip_Till_Line;
            //cout<<skip_Till_Line<<endl;
            sno=0;
            while(getline(dsFile,line)){
                if(currentLine<skip_Till_Line){
                    currentLine++;
                    continue;
                }
                else if(line.substr(0,line.find(':'))=="Date"){       //as this line won't even let you run the function if you didn't use searchDate()
                    cout<<"Desired serial number not found in the entered date"<<endl;
                    found_sno=false;
                    break;
                }
                sno=stoi(line.substr(0,line.find(' ')));

                if (sno==d_sno) {
                    return addAtLine;
                }
                addAtLine++;
            }
            if(sno==0){
                found_sno=false;
                cout<<"Desired serial number not found in the record."<<endl;       //change to no data found corresponding to entered date
            }
            else if (dsFile.eof() && !dsFile.fail()) {
                found_sno=false;
                cout << "Desired serial number not found in the file."<<endl;
            }
            else if (dsFile.fail()) {
                found_sno=false;
                cout << "Error occurred while reading the file."<<endl;
            }
            dsFile.close();
            return -1;
        }

        void preGetBetweenLines(int lineIndex){
            uFile.open(name,ios::in);
            while (getline(uFile, line)) {
                lines.push_back(line);
            }
            uFile.close();
            uFile.open(name,ios::out);
            for (int i = 0; i < lineIndex/*addAtLine*/ && i < lines.size(); ++i) {
                uFile<<lines[i]<<endl;
            }
        }

        void postGetBetweenLines(int lineIndex){
            for (int i = lineIndex/*addAtLine*/; i < lines.size(); ++i) {
                uFile<<lines[i]<<endl;
            }
            uFile.close();
            lines.clear();
            lines.shrink_to_fit();
        }

        void getEntryInVariables(){
                currentLine=0;
                string word;
                uFile.open(name,ios::in);
                while(getline(uFile,line)){
                    if(currentLine<(addAtLine-1)){
                        currentLine++;
                        continue;
                    }
                    else{
                        for(int i=0;i<6;i++){
                            uFile>>word;
                            if(i==0){
                                sno=stoi(word);
                            }
                            else if(i==1){
                                item_name=word;
                            }
                            else if(i==2){
                                qty=stoi(word);
                            }
                            else if(i==3){
                                price=stof(word);
                            }
                            else if(i==4){
                                tprice=stof(word);
                            }
                            else if(i==5){
                                category=word;
                            }
                        }
                        break;
                    }
                }
                uFile.close();
        }

        void reassignSno(){     //used when the serial number is +1 than it's original value, should be called with searchSno. Used in deleteEntry function
            fstream wFile;
            int increase_line=0;
            currentLine=0;
            string word;
            bool exitfunction =false;
            wFile.open(name,ios::in);
            while(getline(wFile,line)){
                //cout<<"current line="<<currentLine<<" addAtLine="<<addAtLine<<endl;       used for debugging
                if(currentLine<(addAtLine-1)){
                    currentLine++;
                    continue;
                }
                else if(line.substr(0,line.find(':'))=="Date"&&currentLine!=addAtLine-1){
                    //cout<<"line break"<<endl;
                    break;
                }
                else if(wFile.eof()&&wFile.fail())
                    break;
                else{
                    for(int i=0;i<6;i++){
                        wFile>>word;
                        if(word.substr(0,word.find(':'))=="Date"){
                            //cout<<"word break"<<endl;
                            exitfunction=true;
                            break;
                        }
                        else if(wFile.eof()&&wFile.fail()){
                            exitfunction=true;
                            break;
                        }
                        if(i==0){
                            sno=stoi(word);
                            //cout<<"ok"<<endl;
                        }
                        else if(i==1){
                            item_name=word;
                        }
                        else if(i==2){
                            qty=stoi(word);
                        }
                        else if(i==3){
                            price=stof(word);
                        }
                        else if(i==4){
                            tprice=stof(word);
                        }
                        else if(i==5){
                            category=word;
                        }
                    }
                    if(exitfunction==true){
                        break;
                    }
                    sno-=1;
                    entry.clear();
                    entry=to_string(sno)+" "+item_name+" "+to_string(qty)+" "+to_string(price)+" "+to_string(tprice)+" "+category;
                    preGetBetweenLines(addAtLine+increase_line);
                    uFile<<entry<<endl;
                    postGetBetweenLines(addAtLine+increase_line+1);
                    increase_line++;
                    currentLine++;
                }
            }
            wFile.close();
        }

        void calculateExpenses(){
            float temp;
            string word;
            ntotal=0,wtotal=0,stotal=0,relative_stotal=0;
            //cout<<"Enter month and year: "<<endl;
            //cin>>dateM>>dateY;
            for(int i=1;i<32;i++){
                dateD=i;
                date=to_string(dateD)+" "+to_string(dateM)+" "+to_string(dateY);
                skipTillLine=searchDate(date)+1;
                if(found_D==true){
                    uFile.open(name,ios::in);
                    searchMaxSno(skipTillLine);
                    currentLine=0;
                    while(getline(uFile,line)){
                        if(currentLine<skipTillLine-1){
                            currentLine++;
                            continue;
                        }
                        else if(currentLine==addAtLine-1){
                            break;
                        }
                        else{
                            for(int j=0;j<6;j++){
                                uFile>>word;
                                //cout<<word<<endl;
                                if(uFile.eof()){
                                    break;
                                }
                                else if(j==4){
                                    temp=stof(word);
                                }
                                else if(j==5){
                                    if(word=="N"){
                                        ntotal+=temp;
                                    }
                                    else if(word=="W"){
                                        wtotal+=temp;
                                    }
                                    else if(word=="S"){
                                        stotal+=temp;               //will anyone even use this category?, I didn't even use this variable anywhere
                                    }
                                }
                            }
                            currentLine++;
                        }
                    }
                    uFile.close();
                }
            }
            relative_stotal=Income-ntotal-wtotal;
        }

        void warnUser(){
            getPlanAndIncome();
            calculateExpenses();
            float nper,wper,sper;
            nper=Income*((float)Needs/100);
            wper=Income*((float)Wants/100);
            sper=Income*((float)Savings/100);
            if(ntotal>nper){
                cout<<"Needs Category Status: Warning!! Overspending under needs category found, Be sure to plan your expenses properly"<<endl;
            }
            else{
                cout<<"Needs Category Status: Nice, you planned your necessities properly"<<endl;
            }
            cout<<"\n";
            system("PAUSE");
            cout<<"\n";
            if(wtotal>wper){
                cout<<"Wants Category Status: Warning!! Overspending under wants category found, Be sure to control your urges next time"<<endl;
            }
            else{
                cout<<"Wants Category Status: Good job, you spent in moderation"<<endl;
            }
            cout<<"\n";
            system("PAUSE");
            cout<<"\n";
            if(relative_stotal>=sper){
                cout<<"Savings Category Status: Target met and you saved a total of: "<<relative_stotal<<" Great job!!"<<endl;
            }
            else{
                cout<<"Savings Category Status: Warning!! Savings target not met"<<endl;
            }
            cout<<"\n";
            system("PAUSE");
            system("CLS");
        }

        void displayPieChart(){
            //install gnuPlot ig
        }
    
    public:

        uData(){
            uFile.open("resources/start_screen.txt",ios::in);
            while(getline(uFile,line)){
                cout<<line<<endl;
            }
            uFile.close();
            system("PAUSE");
            system("CLS");
            userD:
            cout<<"Are you a existing user Y/N : ";
            cin>>choice;
            system("CLS");
            if(choice=='N'||choice=='n'){
                cout<<"Enter your name to create an account : ";
                cin>>name;
                name=name+".txt";
                cout<<"Enter your income: ";
                cin>>Income;
                cout<<"Set your budget plan Needs, Wants and Savings respectively (the default plan is 50 30 20 percent): ";
                cin>>Needs>>Wants>>Savings;
                setPlanAndIncome(Income,Needs,Wants,Savings);
            }
            else if(choice=='Y'||choice=='y'){
                cout<<"Enter your username : ";
                cin>>name;
                system("CLS");
                name=name+".txt";
                uFile.open(name,ios::in);
                if(uFile.is_open()){
                    cout<<"Welcome back!!"<<endl;
                    uFile.close();
                }
                else{
                    cout<<"user doesn't exist! please create a new account"<<endl;
                    system("PAUSE");
                    system("CLS");
                    goto userD;
                }
            }
            else{ 
                cout<<"Please answer in Y or N only"<<endl;
                system("PAUSE");
                system("CLS");
                goto userD;
            }
        }

        void addData(){
            char aChoice='Y';
            for_add=true;
            system("CLS");
            getDate();
            if(valid_D==true){
                skipTillLine=searchDate(date)+1;            //maybe remove these two lines and directly change the variables
                searchMaxSno(skipTillLine);
                preGetBetweenLines(addAtLine);
            
                do{
                    system("CLS");
                    cout<<"Enter item name(without spaces): ";
                    cin>>item_name;
                    cout<<"Enter Quantity: ";
                    cin>>qty;
                    cout<<"Enter Price: ";
                    cin>>price;
                    cout<<"Enter budget category(N,W,S): ";
                    cin>>category;
                    if(category=="N"||category=="W"||category=="S"){
                        tprice=(float)qty*price;
                        sno++;
                        entry=to_string(sno)+" "+item_name+" "+to_string(qty)+" "+to_string(price)+" "+to_string(tprice)+" "+category;       //category is remaining(savings,spendings,neccessity)
                        if (uFile.is_open() && !uFile.eof()) {
                            uFile << entry << endl;
                            cout<<"entry added successfully."<<endl;
                        }
                        else {
                            cout<<"Failed to save entry." <<endl;
                        }
                        cout<<"Do you want to add another entry? Y/N : ";
                        cin>>aChoice;
                    }
                    else{
                        cout<<"Please enter correct category! note that category is case sensetive, returning to main menu"<<endl;
                        system("PAUSE");
                        system("CLS");
                        break;
                    }
                }while(aChoice=='Y'||aChoice=='y');
                if(aChoice=='N'||aChoice=='n'||aChoice=='Y'||aChoice=='y'){
                    system("PAUSE");
                    system("CLS");
                }
                else{
                    cout<<"Please enter N only to stop adding entries, returning to main menu"<<endl;
                    system("PAUSE");
                    system("CLS");
                }
                postGetBetweenLines(addAtLine);
            }
        }

        void displayData(){
            for_add=false;
            int display_choice;
            string word;
            getPlanAndIncome();
            system("CLS");
            cout<<" "<<setfill('_')<<setw(33)<<""<<setfill(' ')<<endl;
            cout<<"|"<<setw(23)<<right<<"Display Menu"<<setw(11)<<"|"<<endl;
            cout<<"|"<<setfill('-')<<setw(34)<<"|"<<setfill(' ')<<endl;
            cout<<"|1.Display records of a single day|"<<endl;
            cout<<"|2.Display record of a whole month|"<<endl;
            cout<<"|"<<setfill('-')<<setw(34)<<"|"<<endl;
            cout<<"|"<<setfill('_')<<setw(34)<<"|"<<setfill(' ')<<endl;
            cout<<"\nEnter choice number: ";
            cin>>display_choice;
            system("CLS");
            if(display_choice==1){
                getDate();
                if(valid_D==true){
                    system("CLS");
                    skipTillLine=searchDate(date)+1;
                    if(found_D==true){
                        searchMaxSno(skipTillLine);
                        uFile.open(name);
                        currentLine=0;
                        cout<<" "<<setfill('_')<<setw(96)<<""<<setfill(' ')<<endl;
                        cout<<left<<"|Income: "<<setw(88)<<Income<<"|"<<endl;
                        cout<<setw(97)<<"|Plan"<<"|"<<endl;
                        cout<<setw(30)<<"|Needs: "+to_string(Needs)+"%"<<setw(30)<<"Wants: "+to_string(Wants)+"%"<<setw(37)<<"Savings: "+to_string(Savings)+"%"<<"|"<<endl;
                        //cout<<setfill('-')<<setw(7)<<"|"<<setw(40)<<"|"<<setw(12)<<"|"<<setw(12)<<"|"<<setw(15)<<"|"<<setw(11)<<"|"<<"|"<<setfill(' ')<<endl;
                        cout<<setw(97)<<"|Date: "+date<<"|"<<endl;
                        cout<<"|"<<setfill('_')<<setw(96)<<""<<"|"<<endl;
                        cout<<setfill('-')<<setw(7)<<"|"<<setw(40)<<"|"<<setw(12)<<"|"<<setw(12)<<"|"<<setw(15)<<"|"<<setw(11)<<"|"<<"|"<<setfill(' ')<<endl;
                        cout<<setw(7)<<"|Sno "<<setw(40)<<"| Item name "<<setw(12)<<"| Quantity "<<setw(12)<<"| Price "<<setw(15)<<"| Total Price "<<setw(11)<<"| Category |"<<endl;
                        while(getline(uFile,line)){
                            if(currentLine<skipTillLine-1){
                                currentLine++;
                                continue;
                            }
                            else if(currentLine==addAtLine-1){
                                break;
                            }
                            else{
                                for(int i=0;i<6;i++){
                                        uFile>>word;
                                        if(uFile.eof()){
                                            break;
                                        }
                                        else if(i==0){
                                            sno=stoi(word);
                                        }
                                        else if(i==1){
                                            item_name=word;
                                        }
                                        else if(i==2){
                                            qty=stoi(word);
                                        }
                                        else if(i==3){
                                            price=stof(word);
                                        }
                                        else if(i==4){
                                            tprice=stof(word);
                                        }
                                        else if(i==5){
                                            category=word;
                                        }
                                    }
                                if(!uFile.eof()){
                                    cout<<"|"<<setw(6)<<sno<<"|"<<setw(39)<<item_name<<"|"<<setw(11)<<qty<<"|"<<setw(11)<<price<<"|"<<setw(14)<<tprice<<"|"<<setw(10)<<category<<"|"<<endl;
                                    currentLine++;
                                }
                            }
                        }
                        cout<<setfill('-')<<setw(7)<<"|"<<setw(40)<<"|"<<setw(12)<<"|"<<setw(12)<<"|"<<setw(15)<<"|"<<setw(11)<<"|"<<"|"<<endl;
                        cout<<setfill('_')<<setw(7)<<"|"<<setw(40)<<"|"<<setw(12)<<"|"<<setw(12)<<"|"<<setw(15)<<"|"<<setw(11)<<"|"<<"|"<<endl;
                        uFile.close();
                        system("PAUSE");
                        system("CLS");
                        warnUser();
                    }
                    else{
                        cout<<"The entered date doesn't have any records, returning to main menu"<<endl;
                        system("PAUSE");
                        system("CLS");
                    }
                }
            }
            else if(display_choice==2){
                cout<<"Enter month and year: ";
                cin>>dateM>>dateY;
                if(dateM<1 || dateM>12){
                    cout<<"Invalid Month"<<endl;
                    system("PAUSE");
                    system("CLS");
                }
                else if(dateY<1 || dateY>9999){
                    cout<<"Invalid Year"<<endl;
                    system("PAUSE");
                    system("CLS");
                }
                else{
                    system("CLS");
                    cout<<" "<<setfill('_')<<setw(96)<<""<<setfill(' ')<<endl;
                    cout<<left<<"|Income: "<<setw(88)<<Income<<"|"<<endl;
                    cout<<setw(97)<<"|Plan"<<"|"<<endl;
                    cout<<setw(30)<<"|Needs: "+to_string(Needs)+"%"<<setw(30)<<"Wants: "+to_string(Wants)+"%"<<setw(37)<<"Savings: "+to_string(Savings)+"%"<<"|"<<endl;
                    cout<<"|"<<setfill('-')<<setw(96)<<""<<"|"<<setfill(' ')<<endl;
                    cout<<"|"<<setfill('_')<<setw(96)<<""<<"|"<<setfill(' ')<<endl;
                    for(int i=1;i<32;i++){
                        dateD=i;
                        date=to_string(dateD)+" "+to_string(dateM)+" "+to_string(dateY);
                        skipTillLine=searchDate(date)+1;
                        if(found_D==true){
                            uFile.open(name,ios::in);
                            cout<<" "<<setfill('_')<<setw(96)<<""<<setfill(' ')<<endl;
                            cout<<setw(97)<<"|Date: "+date<<"|"<<endl;
                            cout<<"|"<<setfill('_')<<setw(96)<<""<<"|"<<endl;
                            cout<<setfill('-')<<setw(7)<<"|"<<setw(40)<<"|"<<setw(12)<<"|"<<setw(12)<<"|"<<setw(15)<<"|"<<setw(11)<<"|"<<"|"<<setfill(' ')<<endl;
                            cout<<setw(7)<<"|Sno "<<setw(40)<<"| Item name "<<setw(12)<<"| Quantity "<<setw(12)<<"| Price "<<setw(15)<<"| Total Price "<<setw(11)<<"| Category |"<<endl;
                            searchMaxSno(skipTillLine);
                            currentLine=0;
                            while(getline(uFile,line)){
                                if(currentLine<skipTillLine-1){
                                    currentLine++;
                                    continue;
                                }
                                else if(currentLine==addAtLine-1){
                                    break;
                                }
                                else{
                                    for(int i=0;i<6;i++){
                                        uFile>>word;
                                        if(uFile.eof()){
                                            break;
                                        }
                                        else if(i==0){
                                            sno=stoi(word);
                                        }
                                        else if(i==1){
                                            item_name=word;
                                        }
                                        else if(i==2){
                                            qty=stoi(word);
                                        }
                                        else if(i==3){
                                            price=stof(word);
                                        }
                                        else if(i==4){
                                            tprice=stof(word);
                                        }
                                        else if(i==5){
                                            category=word;
                                        }
                                    }
                                }
                                if(!uFile.eof()){
                                    cout<<"|"<<setw(6)<<sno<<"|"<<setw(39)<<item_name<<"|"<<setw(11)<<qty<<"|"<<setw(11)<<price<<"|"<<setw(14)<<tprice<<"|"<<setw(10)<<category<<"|"<<endl;
                                    currentLine++;
                                }
                            }
                        cout<<setfill('-')<<setw(7)<<"|"<<setw(40)<<"|"<<setw(12)<<"|"<<setw(12)<<"|"<<setw(15)<<"|"<<setw(11)<<"|"<<"|"<<endl;
                        cout<<setfill('_')<<setw(7)<<"|"<<setw(40)<<"|"<<setw(12)<<"|"<<setw(12)<<"|"<<setw(15)<<"|"<<setw(11)<<"|"<<"|"<<setfill(' ')<<endl;
                        uFile.close();
                        }
                    }
                //cout<<setfill('-')<<setw(7)<<"|"<<setw(40)<<"|"<<setw(12)<<"|"<<setw(12)<<"|"<<setw(15)<<"|"<<setw(11)<<"|"<<"|"<<endl;
                //cout<<setfill('_')<<setw(7)<<"|"<<setw(40)<<"|"<<setw(12)<<"|"<<setw(12)<<"|"<<setw(15)<<"|"<<setw(11)<<"|"<<"|"<<endl;
                system("PAUSE");
                system("CLS");
                warnUser();
                }
            }
            else{
                cout<<"Choose from above option only, returning to main menu"<<endl;
                system("PAUSE");
                system("CLS");
            }
        }

        void editEntry(){
            for_add=false;
            int changeChoice;
            char eChoice='Y';
            dsno=0;
            system("CLS");
            getDate();
            //system("CLS");
            if(valid_D==true){
                skipTillLine=searchDate(date)+1;
                if(found_D==true){
                    while(eChoice=='Y'){
                        cout<<"Enter the serial number of entry you want to edit: ";
                        cin>>dsno;
                        searchSno(dsno, skipTillLine);
                        
                        if(found_sno==true){
                            getEntryInVariables();
                            entry.clear();
                            entry=to_string(sno)+" "+item_name+" "+to_string(qty)+" "+to_string(price)+" "+to_string(tprice)+" "+category;
                            cout<<"The entry is: "<<entry<<endl;
                            system("PAUSE");
                            system("CLS");
                            cout<<"What would you like to change?"<<endl;
                            cout<<"1.name"<<endl;
                            cout<<"2.qty"<<endl;
                            cout<<"3.price"<<endl;
                            cout<<"4.category"<<endl;
                            cout<<"Enter choice number: ";
                            cin>>changeChoice;
                            system("CLS");
                            switch (changeChoice){
                                case 1:
                                    item_name.clear();
                                    cout<<"Enter new item name: ";
                                    cin>>item_name;
                                    break;
                                case 2:
                                    cout<<"Enter new quantity: ";
                                    cin>>qty;
                                    break;
                                case 3:
                                    cout<<"Enter new price: ";
                                    cin>>price;
                                    break;
                                case 4:
                                    cout<<"Enter new category: ";
                                    cin>>category;
                                    break;
                                default:
                                    cout<<"Please from the above options only, returning back to edit menu."<<endl;
                                    system("PAUSE");
                                    system("CLS");
                                    break;
                                }
                            system("CLS");
                            entry.clear();
                            tprice=qty*price;
                            entry=to_string(sno)+" "+item_name+" "+to_string(qty)+" "+to_string(price)+" "+to_string(tprice)+" "+category;
                            preGetBetweenLines(addAtLine);
                            uFile<<entry<<endl;
                            postGetBetweenLines(addAtLine+1);
                            if(changeChoice==1||changeChoice==2||changeChoice==3||changeChoice==4){
                                cout<<"The entry is changed to: "<<entry<<endl;
                                system("PAUSE");
                                system("CLS");
                            }
                            else{
                                cout<<"The entry failed to change was: "<<entry<<endl;
                                system("PAUSE");
                                system("CLS");
                            }
                        }
                    cout<<"Do you want to edit an entry in the same date?(Y/N): ";
                    cin>>eChoice;
                    system("CLS");
                    }
                }
                else{
                    cout<<"Entered date doesn't have any records, returning to main menu"<<endl;
                    system("PAUSE");
                    system("CLS");
                }
            }
        }

        void deleteRecord(){
            int dChoice;
            string confiremDelete="Y";
            system("CLS");
            cout<<" "<<setfill('_')<<setw(41)<<""<<setfill(' ')<<endl;
            cout<<"|"<<setw(26)<<right<<"Delete Menu"<<setw(16)<<"|"<<endl;
            cout<<"|"<<setfill('-')<<setw(42)<<"|"<<setfill(' ')<<endl;
            cout<<setw(42)<<left<<"|1.Delete a single entry"<<"|"<<endl;
            cout<<"|2.Delete all entries of a particular date|"<<endl;
            cout<<setfill('-')<<setw(42)<<"|"<<"|"<<endl;
            cout<<setfill('_')<<setw(42)<<"|"<<"|"<<setfill(' ')<<endl;
            cout<<"\nEnter the option number: ";
            cin>>dChoice;
            if(dChoice==1){
                system("CLS");
                getDate();
                system("CLS");
                if(valid_D==true){
                    dsno=0;
                    skipTillLine=searchDate(date)+1;
                    if(found_D==true){
                        cout<<"Enter the serial number of entry you want to delete: ";
                        cin>>dsno;
                        searchSno(dsno, skipTillLine);
                        getEntryInVariables();
                        entry.clear();
                        entry=to_string(sno)+" "+item_name+" "+to_string(qty)+" "+to_string(price)+" "+to_string(tprice)+" "+category;
                        d_choice:
                        cout<<"The entry is: "<<entry<<endl;
                        cout<<"Confirm to delete (Y/N): ";
                        cin>>confiremDelete;
                        if(confiremDelete=="Y"||confiremDelete=="y"){
                            preGetBetweenLines(addAtLine);
                            postGetBetweenLines(addAtLine+1);
                            reassignSno();
                            cout<<"Entry deleted successfully"<<endl;
                            system("PAUSE");
                            system("CLS");
                        }
                        else if(confiremDelete=="N"||confiremDelete=="n"){
                            cout<<"Deletion canceled"<<endl;
                            system("PAUSE");
                            system("CLS");
                        }
                        else{
                            cout<<"Please answer in Y or N only!!"<<endl;
                            system("PAUSE");
                            system("CLS");
                            goto d_choice;
                        }
                    }
                    else{
                        cout<<"Entered date doesn't exist"<<endl;
                    }
                }
            }
            else if(dChoice==2){
                getDate();
                system("CLS");
                if(valid_D==true){
                    d_choice2:
                    cout<<"Confirm to delete (Y/N): ";
                    cin>>confiremDelete;
                    if(confiremDelete=="Y"||confiremDelete=="y"){
                        skipTillLine=searchDate(date)+1;
                        searchMaxSno(skipTillLine);
                        preGetBetweenLines(skipTillLine-1);
                        postGetBetweenLines(addAtLine);
                        cout<<"Deleted records successfully"<<endl;
                        system("PAUSE");
                        system("CLS");
                    }
                    else if(confiremDelete=="N"||confiremDelete=="n"){
                        cout<<"Deletion canceled"<<endl;
                        system("PAUSE");
                        system("CLS");
                    }
                    else{
                        cout<<"Please answer in Y or N only!!"<<endl;
                        system("PAUSE");
                        system("CLS");
                        goto d_choice2;
                    }
                }
            }
            else{
                cout<<"Please choose from above choices only, returning to main menu"<<endl;
                system("PAUSE");
                system("CLS");
            }
        }
};

int main(){
    int option;
    uData obj1;
    do{
        cout<<" "<<setfill('_')<<setw(29)<<""<<setfill(' ')<<endl;
        cout<<"|"<<setw(17)<<right<<"MENU"<<setw(13)<<"|"<<endl;
        cout<<"|"<<setfill('-')<<setw(29)<<""<<"|"<<setfill(' ')<<endl;
        cout<<setw(30)<<left<<"|1. Display record"<<"|"<<endl;
        cout<<setw(30)<<"|2. Add record"<<"|"<<endl;
        cout<<setw(30)<<"|3. Edit record"<<"|"<<endl;
        cout<<setw(30)<<"|4. Delete record"<<"|"<<endl;
        cout<<setw(30)<<"|5. Exit"<<"|"<<endl;
        cout<<"|"<<setfill('-')<<setw(29)<<""<<"|"<<setfill(' ')<<endl;
        cout<<"|"<<setfill('_')<<setw(29)<<""<<"|"<<setfill(' ')<<endl;
        cout<<"\nEnter your choice number: ";
        
        cin>>option;
        
        if(option==1){
            obj1.displayData();
        }
        else if(option==2){
            obj1.addData();
        }
        else if(option==3){
            obj1.editEntry();
        }
        else if(option==4){
            obj1.deleteRecord();
        }
        else if(option==5){
            break;
        }
        else{
            cout<<"Choose from the above options only"<<endl;
            system("PAUSE");
            system("CLS");
        }
    }while(option!=5);
    system("CLS");
}