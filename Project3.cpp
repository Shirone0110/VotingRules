#include <bits/stdc++.h>
using namespace std;

int num_votes, num_cands, committee_size, marginal[110];
vector < vector < pair <double, int> > > result;
vector < pair <double, double> > voter, cand, bordawin, approvalwin, CCwin;
vector < pair <double, int> > dist;
bool check[110];
vector < pair <int, int> > bordascore, approvalscore;

// voter, cand, bordawin, approvalwin, CCwin contains coordinate
// marginal contains marginal score of each alternative
// result[i] contains distance between voter i and alternative j,
//     the integer is index of alternative for sorting purposes
// dist[j] contains distance between voter and alternative j (reset for each voter)
//     the integer is index of alternative for sorting purposes
// check is boolean array to keep track of member of committee at current time
// bordascore, approvalscore contains corresponding score of each alternative

void generate()
{
    srand(time(NULL));
    random_device rd{};
    //mt19937 gen{rd()};
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen{seed};
    normal_distribution<> d{0, 1};
    for (int i = 0; i < num_votes; i++)
        voter.push_back(make_pair(d(gen), d(gen)));
    for (int i = 0; i < num_cands; i++)
        cand.push_back(make_pair(d(gen), d(gen)));
}

bool comp(pair <double, int> a, pair <double, int> b)
{
    return a.first < b.first;
}

bool compscore(pair <int, int> a, pair <int, int> b)
{
    return a.first > b.first;
}

void BordaApproval()
{
    for (int i = 0; i < num_votes; i++)
        result.push_back({}); // to avoid segfault
    for (int i = 0; i < num_cands; i++)
    {
        // to avoid segfault
        bordascore.push_back(make_pair(0, i));
        approvalscore.push_back(make_pair(0, i));
    }
    for (int i = 0; i < voter.size(); i++)
    {
        dist.clear();
        for (int j = 0; j < cand.size(); j++)
            dist.push_back(make_pair(0, j)); // to avoid segfault
        
        // calculate euclidean distance
        for (int j = 0; j < cand.size(); j++)
        {
            dist[j].first += sqrt((cand[j].first - voter[i].first) * (cand[j].first - voter[i].first)
                + (cand[j].second - voter[i].second) * (cand[j].second - voter[i].second));
            result[i].push_back(make_pair(dist[j].first, j));
        }

        // sort distance to get preference order
        sort(dist.begin(), dist.end(), comp);

        // calculate scores
        for (int j = 0; j < cand.size(); j++)
        {
            // cout << i << " " << j << " " << dist[j].first << '\n';
            bordascore[dist[j].second].first += num_cands - j - 1;
            approvalscore[dist[j].second].first++;
        }
    }
    
    // sort scores to get winners
    sort(bordascore.begin(), bordascore.end(), compscore);
    sort(approvalscore.begin(), approvalscore.end(), compscore);

    for (int i = 0; i < committee_size; i++)
    {
        bordawin.push_back(cand[bordascore[i].second]);
        approvalwin.push_back(cand[approvalscore[i].second]);
    }
}

void marginal_calc()
{
    fill(marginal, marginal + num_cands, 0);
    int best = -1, save = -1;
    for (int i = 0; i < num_votes; i++)
    {
        int flag = num_cands;

        // find the first commmittee member in voter i's preference
        for (int j = 0; j < num_cands; j++)
        {
            if (check[result[i][j].second]) 
            {
                flag = j;
                break;
            }
        }
        
        // add marginal score
        for (int j = flag - 1; j >= 0; j--)
        {   
            marginal[result[i][j].second] += flag - j;
        }
    }

    for (int i = 0; i < num_cands; i++)
    {
        // add the best marginal score candidate
        // cout << i << " " << marginal[i] << '\n';
        if (marginal[i] > best)
        {
            best = marginal[i];
            save = i;
        }
    }

    check[save] = true;
    // cout << save << '\n';
}

void greedy_CC()
{
    fill(check, check + num_cands, false);

    // sort every voter's preference
    for (int i = 0; i < num_votes; i++)
    {
        // for (int j = 0; j < num_cands; j++)
        // {
        //     cout << result[i][j].first << " " << result[i][j].second << '\n';
        // }
        // cout << '\n';
        sort(result[i].begin(), result[i].end(), comp);
        // for (int j = 0; j < num_cands; j++)
        // {
        //     cout << result[i][j].first << " " << result[i][j].second << '\n';
        // }
        // cout << '\n';
    }

    for (int i = 0; i < committee_size; i++)
    {
        marginal_calc();
    }

    for (int i = 0; i < num_cands; i++)
        if (check[i]) 
        {
            CCwin.push_back(cand[i]);
        }
}

int main()
{
    num_votes = 1000;
    num_cands = num_votes / 10;
    committee_size = 7;
    generate();
    BordaApproval();
    // for (int i = 0; i < 20; i++)
    // {
    //     cout << result[i][1].second << " ";
    // }
    greedy_CC();
    ofstream out("voter.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < num_votes; i++)
        out << voter[i].first << " " << voter[i].second << endl;
    out.close();
    ofstream out2("candidate.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < num_cands; i++)
        out2 << cand[i].first << " " << cand[i]. second << endl;
    out2.close();
    ofstream out3("bordawinner.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < committee_size; i++)
        out3 << bordawin[i].first << " " << bordawin[i].second << endl;
    out3.close();
    ofstream out4("approvwinner.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < committee_size; i++)
        out4 << approvalwin[i].first << " " << approvalwin[i].second << endl;
    out4.close();
    ofstream out5("CCwinner.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < committee_size; i++)
        out5 << CCwin[i].first << " " << CCwin[i].second << endl;
    out5.close();
    return 0;
}