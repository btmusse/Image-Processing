# Comparing Image Compression Algorithms

## Abstract

We plan to implement three different image compression algorithms: Huffman coding, DEFLATE, and Lempel–Ziv–Storer–Szymanski (LZSS). First, we will implement these methods using serial algorithms. We will then re-implement them in parallel using MPI. We hope to compare the speedup and efficiency of the three algorithms in order to determine which are more suitable for parallelization.

Problem Description: In our increasingly digital world, the data that we generate is growing at an exponential rate. In order to store and make use of large volumes of data, accurate and efficient compression is of the utmost importance. Our focus for the purpose of this project is on image data and three modern algorithms for compressing raw images into a format that is easily stored.

Before jumping into the problem, it is imperative that we clarify what exactly a digital image is and why we might want to compress one. By definition, a digital image is a numeric representation in two dimensions. In its most raw form, a digital image comes in either raster or vector format. Raster format is what we would typically associate with a photograph and vector images are generally text or computer generated image data. It is not uncommon that images have elements of both types. These raw images are simply files containing number values which represent the brightness of a given color at each point or pixel within the image. In this raw format, an image from a mid to high range camera can contain up to 30 MB of data, large enough to quickly fill up harddrives and

make sending and receiving them difficult. Thus the study of image compression began in order to combat this issue.

Image compression can be broken into two sub categories, lossless and lossy algorithms. For the purpose of this project we have chosen to focus on lossless algorithms in order to make comparisons fair. We plan to implement three image compression algorithms in total. The first two, Huffman coding and Lempel–Ziv–Storer–Szymanski (LZSS), are classical compression algorithms formulated in 1952 and 1982 respectively. The third, DEFLATE, is a combination of the previous two and is widely utilized as it is the standard compression algorithm for the PNG image format.

The DEFLATE algorithm begins by implementing LZSS. LZSS leverages the repetitive nature of image data by searching for duplicate strings of data, removing them, and replacing it with an encoded reference to the first occurence of the data string. As a trivial example take the following sentence and its compressed format:

“Reggie redbird is a redbird.”

“Reggie redbird is a (6,7).”

The second instance of the word redbird is replaced by a reference to the start and length of the first instance. It is important to note that the replacement is only made if the segment is longer than the space required to store the reference and if the repeated segment is within a 32 KiB sliding window in order to keep the distance reference to a 15 bit value.

The second step of the DEFLATE algorithm is an implementation of Huffman coding in order to perform bit reduction. At a high level, this algorithm builds a partial tree dictionary from the data based on how frequently a pattern appears. Then this tree is used to assign a binary encoding to the symbols in the tree where high frequency symbols get smaller values. For example, if we wanted to encode the string “illinois”, we would start by counting the occurrences of each symbol. i:3, l:2, nos:1. Then, we would build the following Huffman tree:

![Huffman tree for Illinois](https://github.com/btmusse/Image-Processing/blob/master/Screenshot%202019-10-21%2019.15.54.png)

Encodings:
i: 00     l: 01     n: 10     s: 101      o: 110

Finally, we would assign encodings to each symbol based on the tree and encode the message as “000101001011000101”. This string of bits can then be decoded by stepping through the same tree used for encoding. Once a leaf node is hit, we have our value. We then reset to the root of the tree and continue decoding the bits.

Intellectual Challenge: Parallelizing a Huffman Coding algorithm comes with its own set of unique issues. The basic principle would be to use data parallelism to split the image into even chunks and have each process count up color values. These would then be reduced into total sums and distributed evenly among the processors. A Huffman tree

would be formed and each process would encode its own “chunk”. For efficiency reasons, Huffman encodings have variable bit lengths (for example, common pixels like black may be encoded into shorter bit strings while uncommon pixels will be longer [see the above Huffman tree image]). This poses a problem in parallelization, as the processors must concatenate their bit encodings into one long encoding. The variable bit lengths, when stored in bytes, will have issues on concatenation because of bit-padding. Bit-shifting may be required during this process to align each bitstream properly - we will have to gauge the effect of this phenomenon when we begin prototyping.

LZSS is, by nature, very sequential - recall that it looks for duplicate bit strings within a 32 KiB “sliding window”. As such, splitting the data up will negatively impact the final compressed size. An algorithm searching the whole bit stream will find more duplicates than a parallel algorithm that splits the bit stream into chunks. While we are doing judgments on speedup and efficiency, it would be interesting to note the final compressed image sizes after running our serial algorithms vs our parallel algorithms. This goes for all three of our algorithms - Huffman Coding, LZSS, and DEFLATE.

As previously mentioned, DEFLATE is a two-stage compression algorithm. LZSS is used to remove duplicate strings, then Huffman coding is used to further compress the new string. This algorithm, then, is fundamentally sequential. While both stages of the algorithm are parallelizable, they must be executed in sequence for maximum compression. It would be interesting to toy with this idea by creating separate LZSS dictionaries and Huffman trees for each processor, thus allowing each processor to perform the entire encoding without being blocked by other processors, and comparing

the implementations’ respective time and space efficiencies. This will be marked as optional in case we feel we do not have enough data to create a robust presentation.

By parallelizing three different image compression methods we hope to have a better understanding not only of image compression in C, but also how to modify established methods to work with multiple processors. We may also conclude that some pre-existing methods do not benefit from the introduction of parallelization.

## Resources

We plan to compare our parallelized algorithms visually. To do so, we will plot our data points - including elapsed time, speedup, and efficiency - in matplotlib using Python. Jupyter lab in conjunction with SageMath or an equivalent IPython kernel will give us the freedom to synthesize and visualize our data for the best possible comparisons. We also have found a C implementation of the Huffman coding on geeksforgeeks.org so we plan to utilize that code base as a starting point when we parallelize the Huffman algorithm.

## Work Plan Schedule

Week of:

10/21: Begin work on serial algorithms.

10/28: Have all three serial versions complete and start working on parallelization

Have each group member work on one algorithm

11/04: Have one fully parallelized algorithm for first project presentation on 11/07 (may require use of sample image data if image uploading is not implemented).

11/11: Start parallelizing second algorithm and finish implementation of image uploading/image data structure in the code.

11/18: Finish second parallel algorithm and begin third parallel algorithm.

11/25: Finish third algorithm and begin work on optimization. Begin comparing elapsed time, efficiency, and speedup using Python, Jupyter Lab, and matplotlib.

12/02: Fix any extraneous bugs and finalize parallelization of all three algorithms. Work on final report.

12/09: Submit final project report.

### Members

Ben Mussell
Donovan Johnston
Griffin Megeff

### References

1. Huffman, David A. “A method for the construction of minimum-redundancy codes.” Proceedings of the IRE 40.9 (1952): 1098-1101.

2. L. Peter Deutsch (1996). DEFLATE Compressed Data Format Specification version 1.3. RFC 1951. Retrieved 2019-10-21.

3. Larmore, L. L., & Przytycka, T. M. (1995). Constructing Huffman Trees in Parallel. SIAM JOURNAL ON COMPUTING. doi: 10.1.1.23.3927

4. Saxena, Sanjay & Sharma, Shiru & Sharma, Neeraj. (2016). Parallel Image Processing Techniques, Benefits and Limitations. Research Journal of Applied Sciences, Engineering and Technology. 12. 223-238. 10.19026/rjaset.12.2324

5. Ziv, Jacob, and Abraham Lempel. "A universal algorithm for sequential data compression." IEEE Transactions on information theory 23.3 (1977): 337-343.

6. Zu, Y. & Hua, B.. (2015). Parallelizing the deflate compression algorithm on GPU. 11. 6159-6170. 10.12733/jcis15020.
