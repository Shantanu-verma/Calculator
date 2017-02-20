#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>	   // For isdigit() , isalpha()
#include <stdlib.h>	   // For atof() , system()
#include <math.h>      // For pow()

using namespace std;

// Also I used `~' for Empty stack token
// Set Option For Displaying Reverse Polish Notation Of Expression
bool DIS_RPN = 0;

// Return true if Its mathematical operator
bool allowed_ops(char &op) {
	switch (op) {
	case '+':case '-':case '*':case '/':case '^':case '.':case '%':
	return true;
	}
	return false;
}

// Return Precedence of Operators
// '-'  '+' => 1
// '*'  '/' => 2
// '^'      => 3
int precedance(char op){
	switch(op){
	case '-':case '+':return 1;
	case '*':case '/':return 2;
	case '^':return 3;
	}
	return 0;
}

bool parencheck(string &expr){

	vector<char> paren;
	uint32_t i = 0;
	char ctok{' '};
	while( i<= expr.size()){

		// For Checking Parenthesis
		ctok = expr[i];
		switch(ctok){
			case '(': paren.push_back('(');
			          break;
			case ')': if(paren.empty()) return false;
					  else paren.pop_back();
			          break;
			}
		// If () Occurs Without Any Numbers in ()
		if(ctok == '(' && expr[i+1] ==')')
					return false;
		// if there is operators without numbers inside Parenthesis
		if((!i && ctok =='(' && allowed_ops(expr[i+1])) || (ctok =='(' && !allowed_ops(expr[i-1]) && i && expr[i-1]!='(') )
					return false;
		else if(ctok == ')'&& allowed_ops(expr[i-1]))
					return false;
		else if(ctok ==')'&& expr[i+1] && expr[i+1]!=')')
					if(!allowed_ops(expr[i+1]))
						return false;
		++i;
	}
	if(!paren.empty())
		return false;
	return true;
}

// Checking Expression For Errors
bool errorf(string &expr) {

	uint32_t i = 0;
	while (i < expr.size()) {
		// Ignoring Parenthesis as its checked by parencheck()
		if((expr[i]!='(' && expr[i]!=')')){
		// Error If alphabets
		if (isalpha(expr[i]))
			return true;
		// Ignore Special Characters
		else if(!allowed_ops(expr[i]) && !isdigit(expr[i]))
			return true;
		// If Continuous Symbols Found
		else if (allowed_ops(expr[i]) && allowed_ops(expr[i + 1]))
			return true;
		// If Only operand without numbers
		else if(allowed_ops(expr[0]) || (allowed_ops(expr[i]) && expr[i+1] == '\0'))
				return true;
		}
		++i;
	}

	if(!parencheck(expr))
		return true;
	return false;
}

// Pop Value From Vector Stack
template<typename Ts = char>
Ts v_pop(vector<Ts> &stack){
	if(stack.empty())
		return '~';
	Ts token = stack.back();
	stack.pop_back();
	return token;
}

// Return top token from stack
char v_top(vector<char> &tok){
	if(tok.empty())
		return '~';
	else
		return tok.back();
}

//Parsing Stuff
bool parser(string &expr){

	// Error Checking
	if (errorf(expr))
			return false;

	// Else main processing
	vector<char> tokenstack;
	string rpn;
	uint32_t it {0};
	char stack_token {' '},infix_token {' '},ctoken {' '};
	int pr_stack_token(0),pr_infix_token(0);
	while(it < expr.size()){
		ctoken = expr[it];

		if(isdigit(ctoken) || ctoken == '.')
			rpn += ctoken;
		else if(ctoken == '(')
			tokenstack.push_back(ctoken);
		else if(ctoken == ')'){
			while( (stack_token = v_pop(tokenstack))!='(' ){
				rpn += ' ';
				rpn += stack_token;
			}
		}
		else if(allowed_ops(ctoken)){
			// Spacing b/w tokens
			rpn += ' ';

		    // Precedence Manager
			infix_token    = ctoken;
			stack_token    = v_top(tokenstack);
			pr_stack_token = precedance(stack_token);
			pr_infix_token = precedance(infix_token);

			// If Infix token Has higher precedence and fix for '^' association
			if(pr_infix_token > pr_stack_token || (pr_infix_token == pr_stack_token && infix_token =='^')){
				tokenstack.push_back(ctoken);
			}
			else{
				while(pr_stack_token >= pr_infix_token && infix_token!='^'){
					if(stack_token == '~') break;
					stack_token = v_pop(tokenstack);
					rpn += stack_token;
					rpn +=' ';
					pr_stack_token = precedance(v_top(tokenstack));
				}
				tokenstack.push_back(infix_token);
			}
		}

		++it;
	}
	rpn+=' ';

	// Remaining Tokens
	while((stack_token = v_pop(tokenstack))!='~'){
		rpn += stack_token;
		rpn += ' ';
	}

	if(DIS_RPN)
	cout<<" RPN : "<<rpn<<'\n';

	// Now Calculations
	vector<double> num;
	it = 0;
	uint32_t fpos = 0,dflag = 0;

	while(it < rpn.size()){

		if(rpn[it] == ' ' && isdigit(rpn[it-1])){
			double n = atof(rpn.substr(fpos,it).data());
			num.push_back(n);
		}
		else if(allowed_ops(rpn[it]) && rpn[it]!='.'){

			double lhs(0),rhs(0);
			rhs = num.back();
		    num.pop_back();
			lhs = num.back();
			num.pop_back();
			switch(rpn[it]){
			case '+': num.push_back(lhs + rhs);break;
			case '-': num.push_back(lhs - rhs);break;
			case '*': num.push_back(lhs * rhs);break;
			case '/': if(rhs!=0) num.push_back(lhs / rhs);
				      else { cout<<" Runtime Error : Division By zero :("; dflag = 1; }; break;
			case '^': num.push_back(pow(lhs,rhs));break;
			default : cout<<"\n Unknown Error ??\n";
			}

		}

		// Giving previous pos to fpos for substr()
		if(rpn[it] == ' ') fpos = it;
		if(dflag) break;
		++it;
	}

	// If Division by zero not occurred
	if(!dflag)
	cout<<" = "<<num.back();
	return true;
}

int main() {

	// Intro Stuff
	string helps = "  \n Help:\n"
				   "  > Type \"clear\" For clearing Screen\n"
				   "  > Support * , / , - , + , ^ and () and float Calculations \n"
				   "  > Type \"exit\" For Exiting \n"
				   "  > Type \"drpn\" for disabling and \"erpn\" for enabling Reverse Polish Notation\n";
	cout<<"\n\t ===========================================\n";
    cout<<  "\t ----- CALCULATOR ( By Shantanu Verma )    -----\n";
	cout<<  "\t ===========================================\n";
	cout<<"\n\t         Type \"help\" for Help               ";

	// User Enters expression
	while( true ){

	cout<<"\n\n Enter:\n > ";
	string expression;
	getline(cin,expression);

	// Checking For Commands
	     if(expression == "exit")
		break;
	else if(expression == "help")
		cout<<helps;
	else if(expression == "clear")
		system("CLS");
	else if(expression == "drpn")
		DIS_RPN = 0;
	else if(expression == "erpn")
		DIS_RPN = 1;

	// Sending To Parser
	else if(!parser(expression))
		cout<<" Parse Error : Unrecongnize Symbol or Sequence of operators :(";
	}

	return 0;
}

