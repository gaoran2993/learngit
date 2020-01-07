#include "longint.h"
#include <iostream>
#include <string>
#include <string.h>
#include <algorithm>

longint operator+(const longint &a,const longint &b){
    if(a=='0'&&b=='0'){
        std::string s0;
        s0+='0';
        return s0;
    }
    if(a.sign==true&&b.sign==false){
        longint b2=b;
        b2.sign=true;
        return a-b2;
    }
    if(a.sign==false&&b.sign==true){
        longint a2=a;
        a2.sign=true;
        return b-a2;
    }
    bool bo=true;
    if(a.sign==false) bo=false;
    std::string str1=a.lint,str2=b.lint,str3;
    int n1 = str1.size(), n2 = str2.size(), n,i,j;
    n=std::max(n1,n2);
    char *c;
    c = new char[n+1]{'0'};
    reverse(str1.begin(),str1.end());
    reverse(str2.begin(),str2.end());
    str1.append(n+1-n1,'0');
    str2.append(n+1-n2,'0');
    int m = 0 ;
    for (i = 0 ; i<n+1; i++ ){
        c[i] = (str1[i]-'0'+str2[i]-'0'+m)%10+'0';
          if (str1[i]-'0'+str2[i]-'0'+m>=10) m = 1 ;
          else m = 0 ;
    }
    for ( i=n;i>=0;--i){
            if ( c[i]!='0') {
                    j = i;break;
            }
    }
    if(i==0)str3=c[0];
    else for (i=j;i>=0;--i){
            str3+=c[i];
        }
    
    longint l;
    l.sign=bo;
    l.lint=str3;
    if(l=='0') l.sign=true;
    delete c;
    return l;
}
std::ostream &operator<<(std::ostream &os,const longint &a){
        if(a.sign==true)
            os<<a.lint;
        else{
            os<<"-"<<a.lint;
        }
        return os;
}
longint operator-(const longint &a,const longint &b){
    if(a=='0'&&b=='0'){
        std::string s0;
        s0+='0';
        return s0;
    }
    if(a.sign==true&&b.sign==false) {
        longint b2=b;
        b2.sign=true;
        return a+b2;
    }
    if(a.sign==false&&b.sign==true){
        longint b2=b;
        b2.sign=false;
        return a+b2;
    }
    bool bo;
    std::string str1, str2, str3;
    if(a>b){
        str1=a.lint;
        str2=b.lint;
        if(a.sign==true) bo=true;
        else bo=false;
    }
    else{
        str1=b.lint;
        str2=a.lint;
        if(a.sign==true) bo=false;
        else bo=true;
    }
    int n = str1.size(),n2=str2.size(),i,j=0;
    reverse(str1.begin(),str1.end());
    reverse(str2.begin(),str2.end());
    str2.append(n-n2+1,'0');
    str1.append(1,'0');
    char *c;
    c=new char[n+1]{'0'};
    for(i=0;i<n;++i){
        if(str1[i]<str2[i]){
            str1[i+1]-=1;
            c[i]=str1[i]+10-str2[i]+'0';
        }
        else{
            c[i]=str1[i]-str2[i]+'0';
        }
    }
    for (i=n-1;i>=0;--i){
            if ( c[i]!='0') {
                j = i;break;
            }
    }
    if(i==0)str3=c[0];
    else for (i=j;i>=0;--i){
            str3+=c[i];}
    
    longint l;
    l.lint=str3;
    l.sign=bo;
    if(l=='0') l.sign=true;
    delete c;
    return l;

}
longint operator*(const longint &a,const longint &b){
    if((a=='0')||(b=='0')){
        std::string s0;
        s0+='0';
        return s0;
    }
    bool bo=true;
    if((a.sign==true&&b.sign==false)||(a.sign==false&&b.sign==true))
        bo=false;
    std::string s1 , s2 ,str3;
    s1=a.lint;
    s2=b.lint;
    int n1 , n2 , i , j , n , N;
    n1 = s1.size();
    n2 = s2.size();
    n=std::max(n1,n2);
    N=n*2 ;
    int *c;
    c=new int[N]{0};
    reverse(s1.begin(),s1.end());
    reverse(s2.begin(),s2.end());
    s1.append(n-n1,'0');
    s2.append(n-n2,'0');
    for (i=0;i<n;++i){
        for (j=0;j<n;++j){
            c[i+j] += (s1[i]-'0')*(s2[j]-'0');
        }
    }
    for (i=0;i<N;++i){
        if ( c[i]>=10) {
            c[i+1]+=c[i]/10 ;
            c[i]=c[i]%10;
        }
    }
     for (i=N-1;i>=0;--i){
            if (c[i]!=0) {
                    j = i;break;
            }
    }
    for (i=j;i>=0;--i){
        str3+=c[i]+'0';}
    longint l;
    l.lint=str3;
    l.sign=bo;
    delete c;
    return l;
}
longint operator/(const longint &a,const longint &b){
    if((a=='0')||b>a){
        std::string s0;
        if((a.sign&&b.sign)||(!a.sign&&!b.sign))s0+='0';
        else {
            s0+='-';
            s0+='1';
        }
        return s0;
    }
    bool bo=true;
    if((a.sign==true&&b.sign==false)||(a.sign==false&&b.sign==true))
        bo=false;
    std::string s1=a.lint,s2=b.lint,str3;
    int n1=s1.size(),n2=s2.size();
    std::string s0=std::string(s1,0,n2-1);
    longint l0=s0,l2=s2;
    int num;
    for(int i=0;i<n1-n2+1;++i){
        num=0;
        l0.lint+=s1[n2+i-1];
        while(l0>l2||l0.lint.compare(l2.lint)==0){
            l0=l0-l2;
            num++;
        }
        if(num==0&&str3.size()==0)continue;
        str3+=num+'0';
    }
    
    longint l;
    l.sign=bo;
    l.lint=str3;
    if(bo==false){
        std::string ss;
        ss+='1';
        longint l0=ss;
        l=l-l0;
    }
    return l;
}
bool operator>(const longint &a,const longint &b){
        if(a.lint.size()>b.lint.size()||(a.lint.size()==b.lint.size()&&a.lint>b.lint)) return true;
        else return false;
}
longint operator%(const longint &a,const longint &b){
    bool bo=true;
    if((a.sign==true&&b.sign==false)||(a.sign==false&&b.sign==true))
        bo=false;
    longint l0=a/b;
    longint l1=a-b*l0;
    return l1;
}
bool operator==(const longint &a,const longint &b){
    if(a.sign==b.sign&&a.lint.compare(b.lint)==0) return true;
    else return false;
}
bool operator==(const longint &a,const char b){
    if(a.lint.size()==1&&a.lint[0]==b) return true;
    else return false;
}