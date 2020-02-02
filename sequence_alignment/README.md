# Sequence Alignment

Fundamental problem in computational genomics. Measure the similarity of two
strings via quality of "best" alignment.
- Extrapolate function from a genome substring
- Similar genomes can reflect proximity in an evolutionary tree

The algorithm must:
1. Assign a cost for every gap that must be inserted to align string
1. Assign a cost to every mismatched character
1. Output the string with the lowest total cost - know as the Needleman-Wunsch
   score (1970)

On two strings with 500 characters, there are 10^125 possible alignments.

