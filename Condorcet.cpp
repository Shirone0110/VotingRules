#include <bits/stdc++.h>
using namespace std;

int voters, alters, profiles, CondoWinner;
int num_voters[15] = {2, 3, 4, 5, 6, 8, 9, 10, 17, 33, 65, 129, 257, 513, 1024};
int num_alters[8] = {2, 3, 4, 5, 6, 7, 8, 9};
vector <int> CopeWinner, BordaWinner, PluralWinner;
int result[15][15], copeland[15][15];
int tally[15][8], tallyborda[15][8], tallyplural[15][8], tallycoincide[15][8];
vector<vector<int>> a(2000000, vector<int>(15, 0));

void Copeland()
{
    bool check[15];
    for (int i = 1; i <= alters; i++)
        for (int j = 1; j <= alters; j++)
            copeland[i][j] = 0;
    for (int i = 0; i < voters; i++)
    {
        fill(check, check + 15, false);
        for (int j = 0; j < alters; j++)
        {
            check[a[i][j]] = true;
            for (int k = 1; k <= alters; k++)
            {
                if (k == a[i][j]) continue;
                if (!check[k]) copeland[a[i][j]][k]++;
                else copeland[a[i][j]][k]--;
            }
        }
    }
    int max = -10, total[10];
    fill(total, total + 10, 0);
    for (int i = 1; i <= alters; i++)
    {
        for (int j = 1; j <= alters; j++)
        {
            if (i == j) continue;
            if (copeland[i][j] > 0) total[i]++;
            else if (copeland[i][j] < 0) total[i]--;
        }
        if (total[i] > max)
        {
            max = total[i];
        }
    }
    CopeWinner.clear();
    for (int i = 1; i <= alters; i++)
        if (total[i] == max) CopeWinner.push_back(i);
    if (max == alters - 1 && CopeWinner.size() == 1) CondoWinner = CopeWinner[0];
}

void Borda()
{
    int x, score[10], maxscore = 0;
    fill(score, score + 10, 0);
    for (int i = 0; i < voters; i++)
    {
        for (int j = 0; j < alters; j++)
        {
            x = a[i][j];
            score[x] += alters - j - 1;
            maxscore = max(maxscore, score[x]);
        }
    }
    BordaWinner.clear();
    for (int i = 1; i <= alters; i++)
    {
        if (score[i] == maxscore) BordaWinner.push_back(i);
    }
}

void Plurality()
{
    int tally[10], best = 0;
    PluralWinner.clear();
    fill(tally, tally + 10, 0);
    for (int i = 0; i < voters; i++)
    {
        tally[a[i][0]]++;
        best = max(tally[a[i][0]], best);
    }
    for (int i = 1; i <= alters; i++)
    {
        if (tally[i] == best) PluralWinner.push_back(i);
    }
}

void IC(int voters, int alters)
{
    vector <int> v;
    v.clear();
    for (int i = 0; i < alters; i++) v.push_back(i + 1);
    for (int i = 0; i < voters; i++)
    {
        srand(time(NULL));
        random_shuffle(v.begin(), v.end());
        for (int j = 0; j < alters; j++)
        {
            a[i][j] = v[j];
        }
    }
}

int main() 
{
    ofstream out("Condorcet.out", ofstream::out | ofstream::trunc);
    int countdown = 1000;
    while (countdown--)
    {
        for (int nv = 0; nv < 15; nv++)
        {
            for (int na = 0; na < 8; na++)
            {
                voters = num_voters[nv];
                alters = num_alters[na];
                IC(voters, alters);
                CondoWinner = -1;
                Copeland();
                Borda();
                Plurality();
                if (CondoWinner != -1)
                {
                    //out << "Condorcet Winner is " << CondoWinner << endl;
                    tally[nv][na]++;
                    for (int i = 0; i < BordaWinner.size(); i++)
                    {
                        //out << "Borda chose Condorcet Winner" << endl;
                        if (BordaWinner[i] == CondoWinner) tallyborda[nv][na]++;
                    }
                    // out << "Borda did not choose Condorcet Winner" << endl;
                    for (int i = 0; i < PluralWinner.size(); i++)
                    {
                        //out << "Plurality chose Condorcet Winner" << endl;
                        if (PluralWinner[i] == CondoWinner) tallyplural[nv][na]++;
                    }
                    // out << "Plurality did not choose Condorcet Winner" << endl;
                }
                else
                {
                    //out << "There is no Condorcet Winner" << endl;
                    bool flag = false;
                    sort(BordaWinner.begin(), BordaWinner.end());
                    sort(CopeWinner.begin(), CopeWinner.end());
                    sort(PluralWinner.begin(), PluralWinner.end());
                    for (int i = 0; i < BordaWinner.size(); i++)
                    {
                        for (int j = 0; j < CopeWinner.size(); j++)
                        {
                            for (int k = 0; k < PluralWinner.size(); k++)
                                if (BordaWinner[i] == CopeWinner[j] 
                                    && CopeWinner[j] == PluralWinner[k])
                                    {
                                        flag = true;
                                        break;
                                    }
                            if (flag) break;
                        }
                        if (flag) break;
                    }
                    if (!flag)
                    {
                        //out << "Borda, Plurality, and Copeland did not choose the same winner" << endl;
                    }
                    else 
                    {
                        // out << "Borda, Plurality, and Copeland chose the same winner" << endl;
                        tallycoincide[nv][na]++;
                    }
                }
            }
        }
    }
    out << "Number of profiles for each combination: 1000" << endl;
    out << "Information of each combination:" << endl;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            out << "Number of voters: " << num_voters[i] << endl;
            out << "Number of alternatives: " << num_alters[j] << endl;
            out << "Number of profiles with Condorcet Winner: " << tally[i][j] << endl;
            out << "\tIn " << tally[i][j] << " times, Borda chose Condorcet Winner " 
                << tallyborda[i][j] << " times" << endl;
            out << "\tIn " << tally[i][j] << " times, Plurality chose Condorcet Winner "
                << tallyplural[i][j] << " times" << endl;
            out << "\tIn " << 1000 - tally[i][j] << " times where Condorcet Winner does not exist,\n"
                << "Borda, Plurality, and Copeland chose the same winner " << tallycoincide[i][j] << " times" << endl;
        }
        out << endl;
    }
    out.close();
    return 0;
}