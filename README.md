# election-sim
Simulates 2020 U.S. Presidential Election if the only candidates were Biden and Trump. 

The program either recieves user input or uses default turnover percentages. The default turnover for Jorgensen is: 30% for Biden, 15% for Trump, 55% Non-voters. Default turnover for Hawkins is: 30% for Biden, 5% for Trump, 65% Non-voters. These values were estimated based on 2016 exit poll data.

The executable can be run with flags --default (-d), --custom (-c), --verbose(-v), and --help (-h). Verbose output prints detailed vote counts for every state, in addition to listing the states that flipped.

State-level election data contained in 1976-2020-president.csv.
