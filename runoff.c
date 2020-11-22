#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
}
candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{
    for (int i = 0; i < candidate_count; ++i)
    {
        if (strcmp(name, candidates[i].name) == 0)
        {
            preferences[voter][rank] = i;
            return true;
        }
    }    
    return false;
}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{
    //Raise votes according to each voter
    bool stop = false;
    for (int i = 0; i < voter_count; ++i)
    {
        // Raise a vote for the first ranked candidate of the i'th voter (if not eliminated ofc..)
        for (int j = 0; j < candidate_count; ++j)
        {
            if (strcmp(candidates[j].name, candidates[preferences[i][0]].name) == 0) 
            {
                if (!candidates[j].eliminated)
                {
                    candidates[j].votes++;
                    break;
                }
                else
                {
                    for (int k = 0; k < candidate_count - 1; ++k)
                    {
                        for (int x = 0; x < candidate_count; ++x)
                        {
                            if ((strcmp(candidates[x].name, candidates[preferences[i][k + 1]].name) == 0) & (!candidates[x].eliminated))
                            {
                                candidates[x].votes++;
                                stop = true;
                                break;
                            }
                        }
                        if (stop)
                        {
                            break;
                        }
                    }
                }
                
            } 
        }    
    }
    return;
}

// Print the winner of the election, if there is one
bool print_winner(void)
{
    int maxvote = 0;
    int maxindex;
    // fetch max voted
    for (int i = 0; i < candidate_count; ++i)
    {
        if (candidates[i].votes > maxvote)
        {
            maxvote = candidates[i].votes;
            maxindex = i;
        }
    }
    // print if there is a winner
    if (maxvote > voter_count / 2)
    {
        printf("%s\n", candidates[maxindex].name);
        return true;
    }
    return false;
}

// Return the minimum number of votes any remaining candidate has
int find_min(void)
{
    int minvote = 101;
    // fetch least voted among remaining contestors
    for (int i = 0; i < candidate_count; ++i)
    {
        if ((!candidates[i].eliminated) & (candidates[i].votes < minvote))
        {
            minvote = candidates[i].votes;
        }
    }
    return minvote;
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{
    // counterA stores the number of all remaining Contestors
    int counterA = 0;
    // counterT stores the number of contestors with minimum votes
    int counterT = 0;
    for (int i = 0; i < candidate_count; ++i)
    {
        if (!candidates[i].eliminated)
        {
            counterA++;
            if (candidates[i].votes == min)
            {
                counterT++;
            }
        }
    }
    if (counterT == counterA)
    {
        return true;
    }
    return false;
}

// Eliminate the candidate (or candidiates) in last place
void eliminate(int min)
{
    for (int i = 0; i < candidate_count; ++i)
    {
        if ((!candidates[i].eliminated) & (candidates[i].votes == min))
        {
            candidates[i].eliminated = true;
        }
    }
    return;
}
