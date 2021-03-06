//
//  Inference.cpp
//  SRI
//
//  Created by Babbie Monahelis on 3/10/17.
//  Copyright © 2017 Babbie Monahelis. All rights reserved.
//

#include "SRI.hpp"
#include "KnowledgeBase.hpp"
#include "RuleBase.hpp"
#include "Parser.hpp"
#include "common.h"


// mutexes
mutex factMutex;
mutex targ_returns;

void factThread(vector<string> mems, int totalMembers, vector<string> factQuery, vector< map<string, string> > * ourFacts)
{
    if(totalMembers != mems.size())
        return;
    
    map <string, string> FactMap;
    
    bool found = true;
    
    for (int i = 0; i < totalMembers; i++)
    {
        string temp = factQuery.at(i);
        string fact = mems[i];
        
        if (FactMap.find(temp) == FactMap.end())
            FactMap[temp] = fact;
        
        else
        {
            
            if(FactMap[temp] != fact)
            {
                found = false;
                break;
            }
        }
    }
    
    factMutex.lock();
    
    if(found)
        ourFacts->push_back(FactMap);
    
    factMutex.unlock();
}

SRI::SRI()
{
    kb = new KnowledgeBase();
    rb = new RuleBase();
    
}

void SRI::inputLine(stringstream & inputStream)
{
    string command; 
    string predicate;
    string junk;
    
    getline(inputStream, command, ' '); //grab the command for the input
    getline(inputStream, junk, '#'); //ignore the comments
    
    stringstream predicatestream(junk);
    getline(predicatestream, predicate);
    
}

void SRI::addFact(string factToAdd)
{
    vector<string> fact = parse->addFact(factToAdd); //send the fact input to parser
    kb->addFact(fact); //add it to the kb
}

void SRI::addRule(string ruleToAdd)
{
    
    map<string, vector<string>> rule = parse->addRule(ruleToAdd); //send the rule input to parser
    rb->addRule(rule); //add it to tho rb
}

void SRI::dumpRF(ostream &os, KnowledgeBase *kb, RuleBase *brules)

{   cout<<"DUMP"<<endl;
    cout<<"dumping into dump.sri"<<endl;
    
    for(auto iteratorx = kb->FactDictionary.begin(); iteratorx != kb->FactDictionary.end(); iteratorx++)
    {
        os<<&iteratorx->second<<endl;
    }
    
    for(auto iteratorx = brules->RuleDictionary.begin(); iteratorx != brules->RuleDictionary.end(); iteratorx++)
    {
        os<<&iteratorx->second<<endl;
    }
}

void SRI::dump(KnowledgeBase *kb, RuleBase *rb)
{
    ofstream out;
    //open file
    out.open("dump.sri");
    dumpRF(out, kb, rb);
    
}

void SRI::drop(string param)
{
 
    
}

void SRI::load(string line)
{
    
    ifstream readFile(line);
    string fileLine;
    
    while(getline(readFile, fileLine))
    {
        stringstream stringsToLoad(fileLine);
        inputLine(stringsToLoad);
    }
}

void SRI::inference(string assoc)
{
    
    vector<string> statements = parse->inference(assoc);
    string relation = statements[0]; //aka the association
    statements.erase(statements.begin());
    
    string inferenceStatement;
    
    stringstream associationStream(assoc);
    
    getline(associationStream,inferenceStatement,')');
    
    getline(associationStream,inferenceStatement, ' ');
    
    getline(associationStream,inferenceStatement);
    
    if(kb->exists(relation)) //if we found the relation within the knowledge base
    {
        vector<map<string,string>> member = inferenceFact(relation, statements); //call inference fact on the fact
        
        for(int i = 0; i < member.size(); i++)
        {
            for(int j = 0; j < statements.size(); j++)
            {
                string found = statements[j];
                cout<< found << ": " << member[i][found] << "\n";
            }
        }
    }
    
}

vector<map <string, string> > SRI::inferenceFact(string assoc, vector<string> & mems)
{
    ThreadContainer * threadsCont = new ThreadContainer();
    
    //bool found = true;
    
    vector<vector<string>> members = kb->findFact(assoc); //get the members of the fact
    
    int totalMembers = mems.size(); //get the total number so we can match them

    vector< map<string,string>> ourFacts; //temp vector to hold the facts with this association

    for (int i = 0; i < members.size(); i++)
    {
        threadsCont->insert( new thread(factThread, members[i], totalMembers, mems, &ourFacts));
    }
    
    threadsCont->executeThreads();
    
    delete(threadsCont);
    
    return ourFacts;
    
}

/*
vector<map<string,string>> SRI::inferenceFact(string assoc, vector<string> & mems)
{
    bool found = true;

    vector<vector<string>> members = kb->findFact(assoc); //get the members of the fact
    
    int totalMembers = mems.size(); //get the total number so we can match them
    
    vector< map<string,string>> ourFacts; //temp vector to hold the facts with this association
    
    for(int i = 0; i < members.size(); i++)
    {
        
        if(totalMembers != members[i].size())
            continue;
        
        map<string,string> factMap; //map of the facts that we found
        
        for(int j = 0; j < totalMembers; j++)
        {
            
            string index = mems[j];
            string fact = members[i][j];
            
            if(factMap.find(index) == factMap.end()) //match the fact with the members
                factMap[index] = fact;
            else
            {
                if(factMap[index] != fact) //break if we can't find it
                {
                    found = false;
                    break;
                }
            }
        }
        
        if(found) //add it to our vector
            ourFacts.push_back(factMap);
    }
    return ourFacts; //return it
}
*/


//still need to implement 
vector<map<string,string>> SRI::inferenceRule(string assoc, vector<string> & mems)
{
    vector<map<string, string>> x;
    
    return x;
}

SRI::~SRI(){
    delete(kb);
    delete(rb);
    delete(parse);
}

