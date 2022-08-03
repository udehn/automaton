#include <iostream>
#include <queue>
#include <set>
#include <map>
#include<vector>

using namespace std;

class Automata{
private:
	set<set<int>> Q;
	set<char> Alphabet;
	multimap<set<int>, pair<char, set<int>>> edges;
	set<int> S;
	set<set<int>> F;
public:
	Automata(){}
	set<int> next_states(const set<int> &q, const char &a) const;
	set<int> next_states(const int &q, const char &a) const;
	void Add(const set<int> &q, const char a, const set<int> &p);
	void Add(const int q, const char a, const int p);
	void Add(const int q, const char a, const set<int> &p);
	void Print();
	void set_S(const set<int> &q);
	void set_S(const int &q);
	void set_F(const int &q);
	void set_F(const set<int> &q);
	void reset_F(const set<set<int>> &q);
	void check();
    set<set<int>> get_Q(){ return Q; };
	Automata* to_DFA();
    Automata* minimize_DFA();
    Automata* trans_dfa();
};

void Automata::Add(const set<int> &q, const char a, const set<int> &p){
	Q.insert(q);
	Q.insert(p);
    Alphabet.insert(a);
	edges.insert({q, {a, p}});
}

void Automata::Add(const int q, const char a, const int p){
	set<int> _q = {q};
	set<int> _p = {p};
	Add(_q, a, _p);
}

void Automata::Add(const int q, const char a, const set<int> &p){
	set<int> _q = {q};
	Add(_q, a, p);
}

void Automata::Print(){
	cout << "Q: ";
    cout << "{";
    for (auto i : Q){
        cout << "{";
        for (int j : i)
            cout << j << ", ";
        cout << "}, ";
    }
    cout << "}" << endl;

	cout << "Alphabet: {";
	for (auto i : Alphabet)
		cout << i << ", ";
	cout << "}" << endl;

	cout << "edges: " << endl;
	for (auto iter = edges.begin(); iter != edges.end(); iter ++){
		cout << "\t( {";
		for (auto i : iter->first)
			cout << i << ", ";
		cout << "}, ";
		if (iter->second.first == '\0')
			cout << "epsilon";
		else
			cout << iter->second.first;
		cout << ") = {";
		for (auto i : iter->second.second)
			cout << i << ", ";
		cout << "}" << endl;
	}

	cout << "S: {";
	for (auto item : S)
		cout << item << ", ";
	cout << "}" << endl;

	cout<<"F :";
    cout << "{";
    for (auto i : F){
        cout << "{";
        for (int j : i)
            cout << j << ", ";
        cout << "}, ";
    }
    cout << "}" << endl;
}

set<int> Automata::next_states(const set<int> &q, const char &a) const
{
	set<int> res;
	for (auto s : q){
		auto item = edges.equal_range({s});
		if (item.first != edges.end())  // 如果找到了这种映射
			for (auto iter = item.first; iter != item.second; iter ++)
				if (iter->second.first == a) // 所有的映射里面寻找通过字符a进行转变的映射
					for (auto status : iter->second.second)  // iter:{status, {sym, status}}
						res.insert(status);
	}
	return res;
}

set<int> Automata::next_states(const int &q, const char &a) const
{
	set<int> _q = {q};
	return next_states(_q, a);
}

Automata *Automata::to_DFA(){
	Automata *DFA = new Automata;
	set<set<int>> tmp_all = {S};
	set<set<int>> for_sign;
	while (!tmp_all.empty()){
		set<int>  l= *tmp_all.begin();
		for_sign.insert(l);
		tmp_all.erase(tmp_all.begin());
		for (auto a : Alphabet){
			set<int> U = { next_states(l,a) };
			if (!U.empty()){
				if (for_sign.find(U) == for_sign.end())
					tmp_all.insert(U);
				DFA->Add(l, a, U);
				if (DFA->S.size() == 0)
					DFA->set_S(l);
			}
		}
	}
	DFA->reset_F(F);
	return DFA;
}

void Automata::set_S(const set<int> &q)
{
	if (Q.find(q) == Q.end())
		return;
	this->S = q;
}

void Automata::set_S(const int &q)
{
	if (Q.find(set<int>{q}) == Q.end())
		return;
	this->S = {q};
}

void Automata::set_F(const int &q)
{
	if(Q.find(set<int>{q}) == Q.end())
		return;
	this->F.insert(set<int>{q});
}

void Automata::set_F(const set<int> &q)
{
	if(Q.find(q) == Q.end())
		return;
	this->F.insert(q);
}

void Automata::reset_F(const set<set<int>> &old_F){
	for(auto itQ : Q){
		bool flag = false;
		for (auto itoF = old_F.begin(); !flag && itoF != old_F.end(); itoF ++)
			for (auto itnF = itoF->begin(); !flag && itnF != itoF->end(); itnF ++)
				if (itQ.find(*itnF) != itQ.end())
					flag = true;
		if (flag)
			F.insert(itQ);
	}
}

void Automata::check(){
    for(auto it : F)
        if(it==S)
            F.erase(it);
}

Automata* Automata::minimize_DFA(){
	Automata *mini_dfa = new Automata;
    queue<vector<set<int>>> have_to_look;
    vector<set<int>> v0;
    vector<set<int>> v1;
    vector<vector<set<int>>> vall;
    map<set<int>, vector<int> > m;
    for(set<set<int>>::iterator itq=Q.begin(); itq!=Q.end(); itq++){
        if(F.find(*itq)==F.end())
            v0.push_back(*itq);
        else
            v1.push_back(*itq);
    }
    vall.push_back(v0);
    vall.push_back(v1);
    have_to_look.push(v0);
    have_to_look.push(v1);

    while(!have_to_look.empty()){
        vector<set<int>> now = have_to_look.front();
        have_to_look.pop();

        for(int i=0; i<now.size(); i++){
            vector<int> grps;
            for(auto a : Alphabet){
                set<int> U = next_states(now[i], a);
                if(U.empty()){
                    grps.push_back(-1);
                    continue;
                }
                for(int i=0; i<vall.size(); i++){
                    for(int j=0; j<vall[i].size(); j++){
                        if(vall[i][j]==U){
                            grps.push_back(i);
                        }
                    }
                }
            }
            m.insert(map<set<int>, vector<int> >::value_type(now[i], grps));
            grps.clear();
        }

/*
        for( map< set<int>, vector<int> >::iterator igf=m.begin(); igf!=m.end(); igf++){
            for(set<int>::iterator ifst=(igf->first).begin(); ifst!=(igf->first).end(); ifst++ )
                cout<<*ifst<<",";
            cout<<" :";
            for(int igrps=0; igrps < (igf->second).size(); igrps++)
                cout<<(igf->second)[igrps]<<"...";
            cout<<endl;
        }
*/

        /// cut set to two subset
        /// Разделить на два множество
        vector<set<int>> vect_all_new_states1;
        vector<set<int>> vect_all_new_states2;
        vector<int> st_flag;
        for(map< set<int>, vector<int> >::iterator im=m.begin(); im!=m.end(); im++){
            int i_st_flag=0;
            if(vect_all_new_states1.empty()){
                vect_all_new_states1.push_back(im->first);
                for( auto aflag : Alphabet ){
                    st_flag.push_back(im->second[i_st_flag]);
                    i_st_flag++;
                }
                continue;
            }
            for(int i_st_flag_st2=0; i_st_flag_st2<Alphabet.size(); i_st_flag_st2++){
                if(im->second[i_st_flag_st2]!=st_flag[i_st_flag_st2]){
                    vect_all_new_states2.push_back(im->first);
                    break;
                } else if(i_st_flag_st2+1 == Alphabet.size() ){
                    vect_all_new_states1.push_back(im->first);
                } else continue;
            }
        }

        if(!vect_all_new_states2.empty()){
            for(vector<vector<set<int>>>::iterator itdel=vall.begin(); itdel!=vall.end(); itdel++){
                if( *itdel == now ){
                    vall.erase(itdel);
                    break;
                }
            }
            vall.push_back(vect_all_new_states1);
            vall.push_back(vect_all_new_states2);
            have_to_look.push(vect_all_new_states1);
            have_to_look.push(vect_all_new_states2);
        }
        vect_all_new_states1.erase(vect_all_new_states1.begin(),vect_all_new_states1.end());
        vect_all_new_states2.erase(vect_all_new_states2.begin(),vect_all_new_states2.end());
        now.erase(now.begin(),now.end());
        m.erase(m.begin(),m.end());
    }

    ///input data to build minimize DFA
    ///использовать данные и минимизировать DFA
    for(int ivall=0; ivall<vall.size(); ivall++){
        for(auto a : Alphabet){
            if(next_states(vall[ivall][0], a).empty())
                continue;
            for(int ivall_2=0; ivall_2<vall.size(); ivall_2++){
                for(int ivall_3=0; ivall_3<vall[ivall_2].size(); ivall_3++){
                    if ( next_states(vall[ivall][0],a) == vall[ivall_2][ivall_3] )
                        mini_dfa->Add(vall[ivall][0], a, vall[ivall_2][0]);
                }
            }
        }
    }
    mini_dfa->set_S(S);
    mini_dfa->reset_F(F);
    mini_dfa->check();
    return mini_dfa;
}


Automata* Automata::trans_dfa(){
    /// use a temp atuomata to build mirror of DFA
    /// использовать временный автомат чтобы построить зеркальный автомат для заданного DFA
    Automata tnfa;
    int tmp1=0, tmp2=0;
    set<int> newend;
    set<int> newstart;
    ///find new_end_state and new_start_state
    ///найти новый состоянии S и F
    for( auto id : edges){
        ///simplify states
        ///упрощать состоянии( здесь просто "объединить" их с помощью операцию "+" )
        ///если более строго, надо обозначим новые состояния новыми символами
        ///но, тип состоянии - это int, поэтому давайте сначала сделаем так
        tmp1=0;
        tmp2=0;
        for(auto isfr : id.first){
            tmp1=tmp1*10+isfr;
        }
        for(auto isto : id.second.second){
            tmp2=tmp2*10+isto;
        }
        tnfa.Add(tmp2,id.second.first,tmp1);
        if(S==id.first)
            newend.insert(tmp1);
        if(S==id.second.second)
            newend.insert(tmp2);
        if(F.find(id.first)!=F.end())
            newstart.insert(tmp1);
        if(F.find(id.second.second)!=F.end())
            newstart.insert(tmp2);
    }

    ///use DFA get his mirror_NFA
    ///использовать данные и построить зеркальный автомат
    Automata* mirror_nfa = new Automata;
    set<set<int>> tmp_all_states;
    queue<set<int>> q;
    q.push(newstart);
    tmp_all_states.insert(newstart);
    set<int> tmp_state_now;
    set<int> tmp_state_to;
    while(!q.empty()){
        tmp_state_now = q.front();
        q.pop();
        for(auto a:Alphabet){
            for( auto iftos : tmp_state_now ){
                set<int> t_iftos;
                t_iftos.insert(iftos);
                for(auto tt_iftos : tnfa.next_states(t_iftos, a))
                    tmp_state_to.insert( tt_iftos );
            }
            mirror_nfa->Add(tmp_state_now, a, tmp_state_to);
            if(tmp_all_states.find(tmp_state_to)==tmp_all_states.end()){
                tmp_all_states.insert(tmp_state_to);
                q.push(tmp_state_to);
            }
            tmp_state_to.clear();
        }
        tmp_state_now.clear();
    }
    mirror_nfa->set_S(newstart);
    set<set<int>> t_newend={newend};
    mirror_nfa->reset_F(t_newend);
    mirror_nfa->check();
    return mirror_nfa;
}

int main()
{
    cout << "----------NFA------------" << endl;
	Automata NFA;
	NFA.Add(1,'a',3);
	NFA.Add(1,'b',2);
	NFA.Add(2,'a',2);
	NFA.Add(2,'b',2);
	NFA.Add(3,'b',1);
	NFA.Add(3,'b',3);
	NFA.set_S(1);
	NFA.set_F(2);
	NFA.set_F(3);
	NFA.Print();

    cout <<endl<< "----------DFA------------" << endl;
	Automata *DFA = NFA.to_DFA();
	DFA->Print();

    cout <<endl<< "--------minimize DFA---------" << endl;
	Automata *mini_dfa = DFA->minimize_DFA();
	mini_dfa->Print();

    cout << "---------R_DFA---------" << endl;
    Automata* R_DFA = mini_dfa->trans_dfa();
    R_DFA->Print();

	delete DFA;
    delete mini_dfa;
    delete R_DFA;
	return 0;
}
