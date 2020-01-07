#ifndef LONGINT_H
#define LONGINT_H

#include<iostream>
#include<string>

class longint{
    friend longint operator+(const longint &a,const longint &b);
    friend std::ostream &operator<<(std::ostream &os,const longint &a);
    friend longint operator-(const longint &a,const longint &b);
    friend longint operator*(const longint &a,const longint &b);
    friend longint operator/(const longint &a,const longint &b);
    friend longint operator%(const longint &a,const longint &b);
    friend bool operator>(const longint &a,const longint &b);//不考虑符号的大小比较
    friend bool operator==(const longint &a,const longint &b);
    friend bool operator==(const longint &a,const char b);
    private:
    std::string lint;
    bool sign;
    public:
    longint(){}
    
    longint(std::string &a){
        if(a[0]=='-') {
            sign=false;
            lint=std::string(a,1);
        }
        else{
            sign=true;
            lint=a;
        }
       
    }
    longint(const std::string &a){
        if(a[0]=='-') {
            sign=false;
            lint=std::string(a,1);
        }
        
        else{
            sign=true;
            lint=a;
        }
       
    }
    longint(const longint &a){
        lint=a.lint;
        sign=a.sign;
    }
    longint(const bool &b){
        if(b==1)lint+='1';
        else lint+='0';
        sign=true;
    }
    operator std::string () const {
        std::string l;
        if(sign==false) l+='-';
        l+=lint;
        return l;
    }
    operator bool () const {
        if(lint.size()==0&&lint[0] =='0')return false;
        else return true;
    } 
    operator double () const {
        int l=lint.size();
        int a,s=0;
        for(int i=0;i<l;++i){
            a=lint[l-i-1]-'0';
            for(int j=0;j<i;++j){
                a=a*10;
            }
            s+=a;
        }
        if(!sign) s=-s;
        return double(s);
    }

};
#endif