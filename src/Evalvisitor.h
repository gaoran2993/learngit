#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3BaseVisitor.h"

#include"longint.h"
#include<map>
#include<vector>
#include<iomanip>
std::map<std::string,antlrcpp::Any>mymap;
std::map<std::string,Python3Parser::SuiteContext*>funsuite;
std::map<std::string,Python3Parser::ParametersContext*>funpa;
std::vector <antlrcpp::Any> vf;
std::vector <std::map<std::string,antlrcpp::Any>> fun;
int f=-1;
class EvalVisitor: public Python3BaseVisitor {

virtual antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override {

    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
    
    std::string str=ctx->NAME()->toString();
    funsuite[str]=ctx->suite();
    funpa[str]=ctx->parameters();
    return 0;
  }

  virtual antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
    
    int n = ctx->tfpdef().size(),n2=ctx->test().size();
    antlrcpp::Any tmp;
    std::string str;
    std::map<std::string,antlrcpp::Any> m;
    fun.push_back(m);
    for(int i=n2-1;i>=0;--i){
      tmp=visit(ctx->test(i-1));
      str=visit(ctx->tfpdef(i+n-n2-1)).as<std::string>();
      if(!fun[f].count(str)) fun[f][str]=tmp;
    }
    for(int i=n-n2-1;i>=0;--i){
      tmp=vf.back();
      str=visit(ctx->tfpdef(i)).as<std::string>();
      fun[f][str]=tmp;
      vf.pop_back();
    }
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
    return ctx->NAME()->toString();
  }

  virtual antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
    return visit(ctx->small_stmt());
  }

  virtual antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
    if(ctx->expr_stmt()) return visit(ctx->expr_stmt());
    else return visit(ctx->flow_stmt());
  }

  virtual antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
    int n=ctx->testlist().size();
    antlrcpp::Any tmp;

    if(!ctx->augassign()&&n>=2){
      int m=ctx->testlist(0)->test().size();
      for(int j=0;j<m;++j){
        tmp=visit(ctx->testlist(n-1)->test(j));
        for(int i=0;i<n-1;++i){
          std::string str=visit(ctx->testlist(i)->test(j)).as<std::string>();
          if(f==-1){
            if(tmp.is<std::string>()&&(mymap.count(tmp.as<std::string>()))){
              tmp=mymap[tmp.as<std::string>()];
            }
            mymap[str]=tmp;}
          else{
            if(tmp.is<std::string>()&&fun[f].count(tmp.as<std::string>())){
              tmp=fun[f][tmp.as<std::string>()];
            }
            fun[f][str]=tmp;
          }
          
        }
      }
      visitChildren(ctx);
    }
    if(ctx->augassign()){
      int mi=10;
      int num[2];
      std::vector <antlrcpp::Any> v0;
      for(int i=0;i<2;++i){
        antlrcpp::Any tmp=visit(ctx->testlist(i));
        if(tmp.is<std::string>()&&mymap.count(tmp.as<std::string>()))
          tmp=mymap[tmp.as<std::string>()];
        if(tmp.is<std::string>()&&fun[f].count(tmp.as<std::string>()))
          tmp=fun[f][tmp.as<std::string>()];
        v0.push_back(tmp);
        if(tmp.is<double>()) {num[i]=0;mi=std::min(mi,num[i]);}
        if(tmp.is<longint>()) {num[i]=1;mi=std::min(mi,num[i]);}
        if(tmp.is<bool>())  {num[i]=2;mi=std::min(mi,1);}
        if(tmp.is<std::string>())  {num[i]=-1;mi=std::min(mi,num[i]);}
      }
    std::vector <double> vd;
    std::vector <longint> vi;
    if(mi==0){
        for(int i=0;i<2;++i){
          switch(num[i]){
            case 0: vd.push_back(v0[i].as<double>());break;
            case 1: vd.push_back(double(v0[i].as<longint>()));break;
            case 2: vd.push_back(double(v0[i].as<bool>()));break;
          }
        }
      }
      if(mi==1){
        for(int i=0;i<2;++i){
          switch(num[i]){
            case 1: vi.push_back(v0[i].as<longint>());break;
            case 2: longint l = v0[i].as<bool>();
                    vi.push_back(l);break;
          }
        }
      }
      antlrcpp::Any ans;
      switch(mi){
            case 0:ans = vd[0];break;
            case 1:ans = vi[0];break;
            case -1:ans = v0[0].as<std::string>();break;
          }
        if(visit(ctx->augassign()).as<int>()==1){
          switch(mi){
            case 0:ans = ans.as<double>()+vd[1];break;
            case 1:ans = ans.as<longint>()+vi[1];break;
            case -1:
              std::string s1,s2;
              s1=ans.as<std::string>();
              s2=v0[1].as<std::string>();
              int n1=s1.size(),n2=s2.size();
              if(s1[0]=='"'||s1[0]=='\'')
                s1=std::string(s1,1,n1-2);
              if(s2[0]=='"'||s2[0]=='\'')
                s2=std::string(s2,1,n2-2);
              
              ans = '"'+s1+s2+'"';
          }
        }
        if(visit(ctx->augassign()).as<int>()==2){
          switch(mi){
            case 0:ans = ans.as<double>()-vd[1];break;
            case 1:ans = ans.as<longint>()-vi[1];break;
          }
        }
        if(visit(ctx->augassign()).as<int>()==3){
          switch(mi){
            case 0:ans = ans.as<double>()*vd[1];break;
            case 1:ans = ans.as<longint>()*vi[1];break;
            case -1:
              std::string s,s0;
              longint count=v0[1].as<longint>();
              s=v0[0].as<std::string>();
                int l=s.size();
                s=std::string(s,1,l-2);
                for(int j=0;j<(int)(double)count;++j)
                  s0+=s;
              ans = '"'+s0+'"';break;
              
          }
        }
        if(visit(ctx->augassign()).as<int>()==4){
          switch(mi){
            case 0:ans = ans.as<double>()/vd[1];break;
            case 1:
            double an = ans.as<longint>();
            an/=double(vi[1]);
            ans=an;break;
          }
        }
        if(visit(ctx->augassign()).as<int>()==5) ans = ans.as<longint>()/vi[1];
        if(visit(ctx->augassign()).as<int>()==6) ans = ans.as<longint>()%vi[1];
      if(f==-1){
          mymap[visit(ctx->testlist(0)).as<std::string>()]=ans;
          return mymap[visit(ctx->testlist(0)).as<std::string>()];}
      else fun[f][visit(ctx->testlist(0)).as<std::string>()]=ans;
          return fun[f][visit(ctx->testlist(0)).as<std::string>()];
    }
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override {
    if(ctx->ADD_ASSIGN()) return 1;
    if(ctx->SUB_ASSIGN()) return 2;
    if(ctx->MULT_ASSIGN()) return 3;
    if(ctx->DIV_ASSIGN()) return 4;
    if(ctx->IDIV_ASSIGN()) return 5;
    if(ctx->MOD_ASSIGN()) return 6;
  }

  virtual antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override {
    if(ctx->break_stmt()) return visit(ctx->break_stmt());
    if(ctx->continue_stmt()) return visit(ctx->continue_stmt());
    if(ctx->return_stmt()) return visit(ctx->return_stmt());
  }

  virtual antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
   return 1;
  }

  virtual antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
    return 2;
  }

  virtual antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
    if(!ctx->testlist()) return "NONE";
    antlrcpp::Any tmp = visit(ctx->testlist());
    if(tmp.is<std::string>()&&fun[f].count(tmp.as<std::string>()))
      tmp=fun[f][tmp.as<std::string>()];
    return tmp;
  }

  virtual antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
    if(ctx->IF()){
      int n=ctx->suite().size();
      if(ctx->ELSE()) n--;
      for(int i=0;i<n;++i){
        bool b=true;
        antlrcpp::Any tmp=visit(ctx->test(i));
        if(tmp.is<bool>()) {
          b=tmp.as<bool>();
        }
        if(tmp.is<double>()) {
          b=(bool)tmp.as<double>();
        }
        if(tmp.is<longint>()) {
          b=(bool)tmp.as<longint>();
        }
        if(tmp.is<std::string>()) {
          std::string s=tmp.as<std::string>();
          if(s.empty()) b=false;
        }
      if(b) return visit(ctx->suite(i));
      }
    if(ctx->ELSE()) return visit(ctx->suite(n));
    else return 0;
      
    }
  return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {

    if(ctx->WHILE()){
      while(1){  
        bool b=true;
        antlrcpp::Any tmp=visit(ctx->test());
        if(tmp.is<bool>()) {
          b=tmp.as<bool>();
        }
        if(tmp.is<double>()) {
          b=(bool)tmp.as<double>();
        }
        if(tmp.is<longint>()) {
          b=(bool)tmp.as<longint>();
        }
        if(tmp.is<std::string>()) {
          std::string s=tmp.as<std::string>();
          if(s.empty()) b=false;
        }
        if(b) {
          int n=ctx->suite()->stmt().size();
          for(int i=0;i<n;++i){
              tmp=visit(ctx->suite()->stmt(i));
              if(tmp.is<int>()){
                if(tmp.as<int>()==1) {b=false;break;}
                if(tmp.as<int>()==2) break;
              }
          }
        }
        if(!b) break;
      }
      return 0;
    }
  return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override {
    if(ctx->simple_stmt()) return visit(ctx->simple_stmt());
    antlrcpp::Any tmp;
    for(int i=0;i<ctx->stmt().size();++i){
      tmp = visit(ctx->stmt(i));
      if(ctx->stmt(i)->simple_stmt())
      if(ctx->stmt(i)->simple_stmt()->small_stmt())
      if(ctx->stmt(i)->simple_stmt()->small_stmt()->flow_stmt())
      if(ctx->stmt(i)->simple_stmt()->small_stmt()->flow_stmt()->return_stmt())
      return tmp;
    }
    return 0;
    
  }

  virtual antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override {
    return visit(ctx->or_test());
  }

  virtual antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override {
    int n = ctx->and_test().size();
    if(n==1) return visit(ctx->and_test(0));
    bool b=true;
    for(int i=0;i<n;++i){
      antlrcpp::Any tmp=visit(ctx->and_test(i));
      if(tmp.is<bool>()) {
        b=tmp.as<bool>();
        if(b)break;
      }
      if(tmp.is<double>()) {
        b=(bool)tmp.as<double>();
        if(b)break;
      }
      if(tmp.is<longint>()) {
        bool b=(bool)tmp.as<longint>();
        if(b)break;
      }
      if(tmp.is<std::string>()) {
        std::string s=tmp.as<std::string>();
        if(s.empty()) b=false;
        if(b)break;
      }
    }
    return b;
  }

  virtual antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override {
    int n = ctx->not_test().size();
    if(n==1) return visit(ctx->not_test(0));
    bool b=true;
    for(int i=0;i<n;++i){
      antlrcpp::Any tmp=visit(ctx->not_test(i));
      if(tmp.is<bool>()) {
        b=tmp.as<bool>();
        if(!b)break;
      }
      if(tmp.is<double>()) {
        b=(bool)tmp.as<double>();
        if(!b)break;
      }
      if(tmp.is<longint>()) {
        b=(bool)tmp.as<longint>();
        if(!b)break;
      }
      if(tmp.is<std::string>()) {
        std::string s=tmp.as<std::string>();
        if(s.empty()) b=false;
        if(!b)break;
      }
    }
    return b;
  }

  virtual antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override {
    if(ctx->NOT()) {
      antlrcpp::Any tmp=visit(ctx->not_test());
      if(tmp.is<bool>()) {
        bool b=tmp.as<bool>();
        return !b;
      }
      if(tmp.is<double>()) {
        bool b=(bool)tmp.as<double>();
        return !b;
      }
      if(tmp.is<longint>()) {
        bool b=(bool)tmp.as<longint>();
        return !b;
      }
      if(tmp.is<std::string>()) {
        std::string s=tmp.as<std::string>();
        bool b=true;
        if(s.empty()) b=false;
        return !b;
      }
    }
    return visit(ctx->comparison());
  }

  virtual antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx) override {
    int n = ctx->arith_expr().size();
    if(n==1) return visit(ctx->arith_expr(0));
    int *num;
    num = new int[n];
      int mi=10;
      std::vector <antlrcpp::Any> v0;
      for(int i=0;i<n;++i){
        antlrcpp::Any tmp=visit(ctx->arith_expr(i));
        if(tmp.is<std::string>()&&mymap.count(tmp.as<std::string>()))
          tmp=mymap[tmp.as<std::string>()];
        if(tmp.is<std::string>()&&fun[f].count(tmp.as<std::string>()))
          tmp=fun[f][tmp.as<std::string>()];
        v0.push_back(tmp);
        if(tmp.is<double>()) {num[i]=0;mi=std::min(mi,num[i]);}
        if(tmp.is<longint>()) {num[i]=1;mi=std::min(mi,num[i]);}
        if(tmp.is<bool>())  {num[i]=2;mi=std::min(mi,1);}
        if(tmp.is<std::string>())  {num[i]=-1;mi=std::min(mi,num[i]);}
      }
      std::vector<double>vd;
      std::vector<longint>vi;
      if(mi==0){
        for(int i=0;i<n;++i){
          switch(num[i]){
            case 0: vd.push_back(v0[i].as<double>());break;
            case 1: vd.push_back(double(v0[i].as<longint>()));break;
            case 2: vd.push_back(double(v0[i].as<bool>()));break;
          }
        }
      }
      if(mi==1){
        for(int i=0;i<n;++i){
          switch(num[i]){
            case 1: vi.push_back(v0[i].as<longint>());break;
            case 2: longint l = v0[i].as<bool>();
                  vi.push_back(l);break;
          }
        }
      }
    bool b=true;
    for(int i=0;i<n-1;++i){
      int sign = visit(ctx->comp_op(i)).as<int>();   
      switch(mi){
        case -1:{
            std::string le=v0[i].as<std::string>(),ri=v0[i+1].as<std::string>();
              switch(sign){
              case 0: if(le!=ri) b=false;break;
              case -1: if(le==ri) b=false;break;
              case 1: if(!(le<ri)) b=false;break;
              case 2:if(!(le>ri)) b=false;break;
              case 3:if(le>ri) b=false;break;
              case 4:if(le<ri) b=false;break;
            }break;}
        case 1:{
          longint le = vi[i] , ri = vi[i+1];
          switch(sign){
            case 0: if(!(le==ri)) b=false;break;
            case -1:if(le==ri) b=false;break;
            case 1: if(!(ri>le)) b=false;break;
            case 2: if(!(le>ri)) b=false;break;
            case 3: if(le>ri) b=false;break;
            case 4: if(ri>le) b=false;break;
          }break;
        }
        case 0:{
          double le = vd[i] , ri = vd[i+1];
          switch(sign){
            case 0: if(!(le==ri)) b=false;break;
            case -1:if(le==ri) b=false;break;
            case 1: if(!(ri>le)) b=false;break;
            case 2: if(!(le>ri)) b=false;break;
            case 3: if(le>ri) b=false;break;
            case 4: if(ri>le) b=false;break;
          }break;
        }

      }
      if(!b)break;
    } 
    delete[]num;
    return b;
  }

  virtual antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override {
    if(ctx->LESS_THAN()) return 1;
    if(ctx->GREATER_THAN()) return 2;
    if(ctx->EQUALS()) return 0;
    if(ctx->NOT_EQ_2()) return -1;
    if(ctx->GT_EQ()) return 4;
    if(ctx->LT_EQ()) return 3;
  }

  virtual antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
    int n=ctx->term().size();
    int n1 = ctx->ADD().size();
    int n2 = ctx->MINUS().size();
    int *num;
    num = new int[n];
    std::string str0 , str;
    for(int i=0;i<10000;++i) str0+='0';
    if(n1+n2>=1){
      for(int i=0;i<n1;++i){
        int s = ctx->ADD(i)->getSymbol()->getTokenIndex();
        str0[s]='+';
        }
      for(int i=0;i<n2;++i){
        int s = ctx->MINUS(i)->getSymbol()->getTokenIndex();
        str0[s]='-';
        }
      for(int i=0;i<10000;++i){
        if(str0[i]!='0'){
          str+=str0[i];
        }
      }
      int mi=10;
      std::vector <antlrcpp::Any> v0;
      for(int i=0;i<n;++i){
        antlrcpp::Any tmp=visit(ctx->term(i));
        if(tmp.is<std::string>()&&mymap.count(tmp.as<std::string>()))
          tmp=mymap[tmp.as<std::string>()];
        if(tmp.is<std::string>()&&fun[f].count(tmp.as<std::string>()))
          tmp=fun[f][tmp.as<std::string>()];
        v0.push_back(tmp);
        if(tmp.is<double>()) {num[i]=0;mi=std::min(mi,num[i]);}
        if(tmp.is<longint>()) {num[i]=1;mi=std::min(mi,num[i]);}
        if(tmp.is<bool>())  {num[i]=2;mi=std::min(mi,1);}
        if(tmp.is<std::string>())  {num[i]=3;mi=std::min(mi,num[i]);}
      }
      std::vector<double>vd;
      std::vector<longint>vi;
      if(mi==0){
        for(int i=0;i<n;++i){
          switch(num[i]){
            case 0: vd.push_back(v0[i].as<double>());break;
            case 1: vd.push_back(double(v0[i].as<longint>()));break;
            case 2: vd.push_back(double(v0[i].as<bool>()));break;
          }
        }
      }
      if(mi==1){
        for(int i=0;i<n;++i){
          switch(num[i]){
            case 1: vi.push_back(v0[i].as<longint>());break;
            case 2: longint l = v0[i].as<bool>();
                  vi.push_back(l);break;
          }
        }
      }
      antlrcpp::Any ans;
      switch(mi){
            case 0:ans = vd[0];break;
            case 1:ans = vi[0];break;
            case 3:ans = v0[0].as<std::string>();break;
          }
        for(int i=0;i<n-1;++i){
        if(str[i]=='+'){
          switch(mi){
            case 0:ans = ans.as<double>()+vd[i+1];break;
            case 1:ans = ans.as<longint>()+vi[i+1];break;
            case 3:
              std::string s1,s2;
              s1=ans.as<std::string>();
              s2=v0[i+1].as<std::string>();
              int n1=s1.size(),n2=s2.size();
              if(s1[0]=='"'||s1[0]=='\'')
                s1=std::string(s1,1,n1-2);
              if(s2[0]=='"'||s2[0]=='\'')
                s2=std::string(s2,1,n2-2);
              
              ans = '"'+s1+s2+'"';
          }
        }
        if(str[i]=='-'){
          switch(mi){
            case 0:ans = ans.as<double>()-vd[i+1];break;
            case 1:ans = ans.as<longint>()-vi[i+1];break;
          }
        }
      }
    delete []num;
    return ans;
    }
    
    return visit(ctx->term(0));
  }

  virtual antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override {

    int n=ctx->factor().size();
    int n1 = ctx->STAR().size();
    int n2 = ctx->DIV().size();
    int n3 = ctx->IDIV().size();
    int n4 = ctx->MOD().size();
    int *num;
    num = new int[n];
    std::string str0 , str;
    for(int i=0;i<10000;++i) str0+='0';
    if(n1+n2+n3+n4>=1){
      for(int i=0;i<n1;++i){
        int s = ctx->STAR(i)->getSymbol()->getTokenIndex();
        str0[s]='*';
        }
      for(int i=0;i<n2;++i){
        int s = ctx->DIV(i)->getSymbol()->getTokenIndex();
        str0[s]='/';
        }
      for(int i=0;i<n3;++i){
        int s = ctx->IDIV(i)->getSymbol()->getTokenIndex();
        str0[s]='!';
        }
      for(int i=0;i<n4;++i){
        int s = ctx->MOD(i)->getSymbol()->getTokenIndex();
        str0[s]='%';
        }
      for(int i=0;i<10000;++i){
        if(str0[i]!='0'){
          str+=str0[i];
        }
      }
      int mi=10;
      std::vector <antlrcpp::Any> v0;
      for(int i=0;i<n;++i){
        antlrcpp::Any tmp=visit(ctx->factor(i));
        if(tmp.is<std::string>()&&mymap.count(tmp.as<std::string>()))
          tmp=mymap[tmp.as<std::string>()];
        if(tmp.is<std::string>()&&fun[f].count(tmp.as<std::string>()))
          tmp=fun[f][tmp.as<std::string>()];
        v0.push_back(tmp);
        if(tmp.is<double>()) {num[i]=0;mi=std::min(mi,num[i]);}
        if(tmp.is<longint>()) {num[i]=1;mi=std::min(mi,num[i]);}
        if(tmp.is<bool>())  {num[i]=2;mi=std::min(mi,1);}
        if(tmp.is<std::string>())  {num[i]=-1;mi=std::min(mi,num[i]);}
      }
    std::vector <double> vd;
    std::vector <longint> vi;
    if(mi==0){
        for(int i=0;i<n;++i){
          switch(num[i]){
            case 0: vd.push_back(v0[i].as<double>());break;
            case 1: vd.push_back(double(v0[i].as<longint>()));break;
            case 2: vd.push_back(double(v0[i].as<bool>()));break;
          }
        }
      }
      if(mi==1||mi==-1){
        for(int i=0;i<n;++i){
          switch(num[i]){
            case -1: vi.push_back(1);break;
            case 1: vi.push_back(v0[i].as<longint>());break;
            case 2: longint l = v0[i].as<bool>();
                    vi.push_back(l);break;
          }
        }
      }
      antlrcpp::Any ans;
      switch(mi){
            case 0:ans = vd[0];break;
            case 1:ans = vi[0];break;
            case -1:ans = vi[0];break;
          }
      for(int i=0;i<n-1;++i){
        if(str[i]=='*'){
          switch(mi){
            case 0:ans = ans.as<double>()*vd[i+1];break;
            case 1:ans = ans.as<longint>()*vi[i+1];break;
            case -1:
              std::string s,s0;
              longint count;
              if(num[i]==-1) {s=v0[i].as<std::string>();count=vi[i+1];}
              if(num[i+1]==-1) {s=v0[i+1].as<std::string>();count=ans.as<longint>();}
              if(ans.is<std::string>()) {s=ans.as<std::string>();count=vi[i+1];}
              if(ans.is<longint>()&&num[i+1]==1) {ans=ans.as<longint>()*vi[i+1];}
              else {
                int l=s.size();
                s=std::string(s,1,l-2);
                for(int j=0;j<(int)(double)count;++j)
                  s0+=s;
              ans = '"'+s0+'"';break;
              }
          }
        }
        if(str[i]=='/'){
          switch(mi){
            case 0:ans = ans.as<double>()/vd[i+1];break;
            case 1:
            double an = ans.as<longint>();
            an/=double(vi[i+1]);
            ans=an;break;
          }
        }
        if(str[i]=='!') {
          longint a=ans.as<longint>(),b=vi[i+1];
          longint c = a/b;
          ans = c;}
        if(str[i]=='%') ans = ans.as<longint>()%vi[i+1];

      }
    delete []num;
    return ans;
    }
    return visit(ctx->factor(0));
  }

  virtual antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override {
    if(ctx->ADD()) return visit(ctx->factor());
    if(ctx->MINUS()){
      antlrcpp::Any tmp=visit(ctx->factor());
      if(tmp.is<double>()) return -1*tmp.as<double>();
      if(tmp.is<longint>()) return tmp.as<longint>()-tmp.as<longint>()-tmp.as<longint>();
    }
    return visit(ctx->atom_expr());
  }

  virtual antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {

    antlrcpp::Any tmp1,tmp2;
    longint l;
    bool b;
    std::string str1,str2;
    if(ctx->trailer()){       
      tmp1=visit(ctx->atom());
      if(tmp1.is<std::string>())
      str1=tmp1.as<std::string>();     
      tmp2=visit(ctx->trailer());
      
      if(str1=="print"){
        int m=ctx->trailer()->arglist()->argument().size();
        for(int j=0;j<m;++j){
          tmp2=visit(ctx->trailer()->arglist()->argument(j));
          if(tmp2.is<std::string>()){
            std::string tmp=tmp2.as<std::string>();
            int s=tmp.size();
            if(tmp[0]=='"'){
            std::string str = std::string(tmp,1,s-2);
            std::cout<<str<<" ";
            }
            else {
              if(f==-1)
                tmp2=mymap[tmp2.as<std::string>()];
              else
                tmp2=fun[f][tmp2.as<std::string>()];
                if(tmp2.is<std::string>()){
                  std::string tmp=tmp2.as<std::string>();
                  int s=tmp.size();
                  if(tmp[0]=='"'){
                  std::string str = std::string(tmp,1,s-2);
                  std::cout<<str<<" ";}
                  else std::cout<<tmp<<" ";
                  }
                }
            }
          if(tmp2.is<longint>())
            std::cout<<tmp2.as<longint>()<<" ";
          if(tmp2.is<double>())
            std::cout<<std::fixed<<std::setprecision(6)<<tmp2.as<double>()<<" ";
          if(tmp2.is<bool>()){
            if(tmp2.as<bool>()==1) std::cout<<"True"<<" ";
            else std::cout<<"False"<<" ";
          }
          if(tmp2.is<int>())
            std::cout<<"None"<<" ";
        }
        std::cout<<std::endl;
      }
      if(f==-1&&tmp2.is<std::string>()&&mymap.count(tmp2.as<std::string>()))
        tmp2=mymap[tmp2.as<std::string>()];
      if(f!=-1&&tmp2.is<std::string>()&&fun[f].count(tmp2.as<std::string>()))
        tmp2=fun[f][tmp2.as<std::string>()];
      if(str1=="int"){
        if(tmp2.is<bool>()) return (int)tmp2.as<bool>();
        if(tmp2.is<std::string>()) {
          longint li=tmp2.as<std::string>();
          return (int)(double)li;
        }
        if(tmp2.is<double>())return (int)tmp2.as<double>();
      }
      if(str1=="float"){
        if(tmp2.is<bool>()) return (double)tmp2.as<bool>();
        if(tmp2.is<std::string>()) {
          longint li=tmp2.as<std::string>();
          return (double)li;
        }
        if(tmp2.is<longint>())return (double)tmp2.as<longint>();
      }
      if(str1=="bool"){
        if(tmp2.is<double>()) return (bool)tmp2.as<double>();
        if(tmp2.is<std::string>()) {
          if(tmp2.as<std::string>().empty()) return true;
          else return false;
        }
        if(tmp2.is<longint>())return (bool)tmp2.as<longint>();
      }
      if(str1=="str"){
        if(tmp2.is<double>()) return std::to_string(tmp2.as<double>());
        if(tmp2.is<bool>()) {
          if(tmp2.as<bool>()) return std::string("1");
          else return std::string("0");
        }
        if(tmp2.is<longint>())return std::string(tmp2.as<longint>());
      }
      if(funsuite.count(str1)){
        int n = ctx->trailer()->arglist()->argument().size(),n2=0;
        antlrcpp::Any tmp ;
        std::string s;
        f++;
        for(int i=0;i<n;++i){
          tmp=visit(ctx->trailer()->arglist()->argument(i)->test());
          if(tmp.is<std::string>()){
            if(f==0&&mymap.count(tmp)) tmp=mymap[tmp.as<std::string>()];
            if(f!=0&&fun[f-1].count(tmp)) tmp=fun[f-1][tmp.as<std::string>()];}
          if(ctx->trailer()->arglist()->argument(i)->NAME()) {
            s=ctx->trailer()->arglist()->argument(i)->NAME()->toString();
            fun[f][s]=tmp;}
          else {
            n2++;
            vf.push_back(tmp);
          }
          }
          tmp = visit(funpa[str1]);
          tmp = visit(funsuite[str1]);
          fun.pop_back();
          f--;
        //  std::cout<<tmp.is<longint>();
          return tmp;
        }
      } 
    else{ 
      tmp2=visit(ctx->atom());
      if(tmp2.is<std::string>()){
        str1=tmp2.as<std::string>();
        return str1;
        }
      if(tmp2.is<double>())
        return tmp2.as<double>();
      if(tmp2.is<longint>()){
          l=tmp2.as<longint>();
          return l;
      }
      if(tmp2.is<bool>()){
        b=tmp2.as<bool>();
        return b;
        }
      if(tmp2.is<int>()){
        return int(0);
      }
      }
    return visitChildren(ctx);
    
   
  }

  virtual antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override {
  
    return visit(ctx->arglist());
  }

  virtual antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override {
    std::string tmp;
    if(ctx->NAME()) {
      tmp = ctx->NAME()->toString();
      return tmp;
      }
    if(ctx->STRING(0)){
      tmp = ctx->STRING(0)->toString();
      return tmp;
      }
    if(ctx->NUMBER()){
      tmp=ctx->NUMBER()->toString();
      int s=tmp.size();
      int dote=0;
      double num;
      for(int i=0;i<s;++i){
        if(tmp[i]=='.') dote=i;
      }
      if(dote==0){
        longint l = tmp;
        return l;
      }
      else{
        double x=1;
        for(int i=dote-1;i>=0;--i){
            num+=(tmp[i]-'0')*x;
            x*=10;
          }
        x=double(1)/10;
        for(int i=dote+1;i<s;++i){
            double n=tmp[i]-'0';
            num+=n*x;
            x/=10;
          }
        return num;
      
      }

        
    }
    if(ctx->TRUE()){
      bool b=true;
      return b;
    }
    if(ctx->FALSE()){
      bool b=false;
      return b;
    }
    if(ctx->NONE()){
      return int(0);
    }
  }

  virtual antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override {
    
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override {

    return visit(ctx->argument(0));
  }

  virtual antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override {
    
    return visit(ctx->test());
  }

};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
