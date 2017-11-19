#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<stack>
#include<algorithm>

using namespace std;


class graph
{
public:
	graph(char* filenev)
	{
		ifstream f(filenev);
		string s;
		VEGTELEN = 0;
		getline(f, s);
		v.resize(atoi(s.c_str()));

		for (int i = 0; i < v.size(); ++i)
		{
			getline(f, s);

			stringstream ss(s);
			string token1;
			string token2;

			getline(ss, token1, ' ');
			int u = atoi(token1.c_str());

			while (getline(ss, token1, ' ') && getline(ss, token2, ' ')) {
				int csucs = atoi(token1.c_str());
				int suly = atoi(token2.c_str());
				v[u] = new node(csucs, suly, v[u]);
				VEGTELEN += suly;
			}
		}
		++VEGTELEN;

		getline(f, s);
		r.resize(atoi(s.c_str()));

		for(int i=0; i<r.size();++i)
		{
			getline(f, s);
			r[i] = atoi(s.c_str());
		}

		f.close();
	}

	~graph()
	{
		for (int i = 0; i < v.size(); ++i)
		{
			while (v[i] != NULL)
			{
				node* p = v[i];
				v[i] = v[i]->next;
				delete p;
			}
		}

		for (int i = 0; i < cs.size(); ++i)
			delete cs[i];
	}

	void szimread(char* fnev)
	{
		ifstream f(fnev);

		int n;
		f >> n;
		k.resize(n);

		for (int i = 0; i < k.size(); ++i)
			f >> k[i].kapacitas >> k[i].start;

		f >> n;
		cs.resize(n);

		for (int i = 0; i < cs.size(); ++i)
		{
			cs[i] = new csomag();
			f >> cs[i]->cel >> cs[i]->suly;
			cs[i]->tavolsag = VEGTELEN;
		}
		f.close();
	}

	void csomagokateloszt()
	{
		sort(cs.begin(), cs.end(), nagyobbcsomag);

		for (int i = 0; i < cs.size(); ++i)
		{
			int maxj = -1;
			for (int j = 0; j < k.size(); ++j)
			{
				if (k[j].szabadhely() >= cs[i]->suly && (maxj==-1 || k[j].szabadhely() > k[maxj].szabadhely()))
					maxj = j;
			}

			if (maxj > -1)
			{
				k[maxj].csomagok.push_back(cs[i]);
				cs[i]->kamion = maxj;
			}
		}
	}

	struct utszakasz
	{
		int hova;
		int tavolsag;
	};

	vector<utszakasz> Dijkstra(int start, int cel)
	{
		vector<int> d;
		vector<int> p;
		DijkstraFa(start, d, p);
		vector<utszakasz> utvonal;
		stack<utszakasz> s;
		while (cel != -1 && cel!=start)
		{
			utszakasz u;
			u.hova = cel;
			u.tavolsag = d[cel] - d[p[cel]];
			s.push(u);
			cel = p[cel];
		}
		while (!s.empty())
		{
			utvonal.push_back(s.top());
			s.pop();
		}
		return utvonal;
	}

	void szimulacio()
	{
		szallitasisorrend();
		for (int i = 0; i < k.size(); ++i)
		{
			int osszut = 0;
			cout << i << "-dik kamion tartalma: suly(cel) " << endl;
			for (int j = 0; j < k[i].csomagok.size(); ++j)
			{
				cout << k[i].csomagok[j]->suly <<"("<<k[i].csomagok[j]->cel<<") ";
			}
			cout << endl<<endl;

			int start = k[i].start;
			cout << i << "-dik kamion utvonala: tavolsag(cel) " << endl;
			cout << start<<" ";
			for (int j = 0; j < k[i].csomagok.size(); ++j)
			{
				vector<utszakasz> ut = Dijkstra(start, k[i].csomagok[j]->cel);
				for (int u = 0; u < ut.size(); ++u)
				{
					cout << ut[u].tavolsag << "(" << ut[u].hova << ") ";
					osszut += ut[u].tavolsag;
				}
				start = k[i].csomagok[j]->cel;
			}
			cout << endl<<endl << "megtett ut: " << osszut << endl;
			cout << endl;
		}
		cout << endl << "ki nem szalitott csomagok: suly(cel)" << endl;
		for (int i = 0; i < cs.size(); ++i)
			if (cs[i]->kamion == -1)
				cout << cs[i]->suly << "(" << cs[i]->cel << ") ";
		cout << endl;
	}

private:

	struct node
	{
		node(int csucs, int  suly, node* next) : csucs(csucs), suly(suly), next(next) {}
		int csucs;
		int suly;
		node* next;
	};

	struct csomag
	{
		int cel;
		int suly;
		int kamion = -1;
		int tavolsag;
	};

	struct kamion
	{
		int kapacitas;
		int start;
		vector<csomag*> csomagok;

		int szabadhely()
		{
			int s = 0;
			for (int i = 0; i < csomagok.size(); ++i)
				s += csomagok[i]->suly;
			return kapacitas - s;
		}
	};

	vector<node*> v;
	vector<int> r;
	vector<kamion> k;
	vector<csomag*> cs;

	int VEGTELEN;

	static bool nagyobbcsomag(csomag* a, csomag* b)
	{
		return a->suly > b->suly;
	}

	static bool kozelebbicsomag(csomag* a, csomag* b)
	{
		return a->tavolsag < b->tavolsag;
	}

	void DijkstraFa(int start, vector<int>& d, vector<int>& p)
	{
		d.resize(v.size(),VEGTELEN);
		p.resize(v.size(), -1);
		vector<bool> kesz(v.size(), false);

		d[start] = 0;
		int i = MinNemKeszCsucs(d, kesz);
		while (i != -1)
		{
			kesz[i] = true;
			for (node* j = v[i]; j != NULL; j = j->next)
			{
				if (d[j->csucs] > d[i] + j->suly)
				{
					d[j->csucs] = d[i] + j->suly;
					p[j->csucs] = i;
				}
			}
			i = MinNemKeszCsucs(d, kesz);
		}
	}

	static int MinNemKeszCsucs(vector<int>& d, vector<bool>& kesz)
	{
		int mini = -1;
		for (int i = 0; i < d.size(); ++i)
		{
			if (!kesz[i] && (mini == -1 || d[i] < d[mini]))
				mini = i;
		}
		return mini;
	}

	void szallitasisorrend()
	{
		for (int i = 0; i < k.size(); ++i)
		{
			vector<int> d,p;
			DijkstraFa(k[i].start, d, p);
			for (int j = 0; j < k[i].csomagok.size(); ++j)
				k[i].csomagok[j]->tavolsag = d[k[i].csomagok[j]->cel];

			sort(k[i].csomagok.begin(), k[i].csomagok.end(),kozelebbicsomag);
		}
	}
};

int main()
{
	graph g("small_graph.txt");
	g.szimread("small_simulation.txt");
	g.csomagokateloszt();
	g.szimulacio();

	return 0;
}
