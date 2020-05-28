# Generation of textual summaries at different target reading levels: 
## summarizing line graphs for visually impaired users 

## Project
This repository contains part of the code used to implement the Interactive_SIGHT system. Interactive_SIGHT (Summarizing Information Graphics Textually) is a system developed to provide sight-impaired individuals with access to information graphics present in multimodal documents from popular media. SIGHT is a Web-based tool that automatically recognizes the high-level knowledge of a graphic and generates natural language text, so screen readers are able to access it. Prior to this work, the SIGHT system was able to process and generate text only for simple bar charts. However, for graphics that are represented with lines and groups of bars, only the message recognition module has been developed.

This code is also the partial requirements for the title of Doctor in Philosophy obtained by Priscilla Moraes and supports the hypothesis presented in this [thesis](http://udspace.udel.edu/handle/19716/19984) and was developed solely by the thesis' author.

## Organization of this repo

Code for the following projects is included in this repo:


### Identification of visual features
In order to generate text that intends to describe an entity, it is essential to first identify its most salient features and components. Knowing the relevance of a feature amongst all options allows one to choose what is important to say and to generate a clear, concise, yet coherent description of the entity. The features of line graphs, or at least what is noticed by users when viewing them in an article, were identified by an experiment performed with human subjects which intended to capture the most salient information conveyed by line graphs.

**Chapter 4**
Indentification of visual features of line graphs
   
**Folder** 
```visual_features``` 
The code is written in Perls scripts and process xml and text files which are the output from the visual recognition phase, which precedes the text generation step and is out of the scope of this work.


### Content determination and text organization
The content selection methodology is based on the PageRank algorithm which is intended to recognize the importance of a node by how connected that node is to other nodes in a graph. In this case, nodes in the graph represent propositions from the weighted features assessed by the identification of visual features phase.
Having chosen what information to communicate, when to say what and which words and syntactic structures best express an intent constitute the full range of the language generation problem nowadays. In order to consistently produce text, one needs to decide on which ordering of sentences to use to be effective regarding its goal of making the discourse coherent. Organization choices are heavily influenced by the content that is available, as well as to the user expectations when answering questions.

**Chapter 5**
Content Determination Phase

**Chapter 6**
Text Organization Phase

**Folder**
```content_definition```
The code is written in C++. There are modules for database connection to read and persist graphics information and sumaries. Classes to implement the content selection and text organization phases, and a primer for the realisation (which was later modified and implemented in Java) using [FUF Surge](https://www.cs.bgu.ac.il/~elhadad/surge) as the surface realizer.

### Micro planning Phase
Prior work has concentrated on simplifying text in order to make it accessible to people with cognitive disabilities or low literacy levels. However, most NLG systems generate text for readers with good reading ability. My contention, however, is that NLG systems will be much more effective if they can target their output to the preferences of the reader. This not only enables easy comprehension, but it also makes the experience more enjoyable for them. With that, I propose an approach that considers a target reading level in order to decide on the syntactic and grammatical structure of the generated text and to select appropriate lexical items.

**Chapter 7**
Micro Planning Phase

**Folder**
```realisation```
The code in this folder is written in Java. The micro planning phase consists of using decision trees and annotated text fromt he [Common Core Standards](http://www.corestandards.org/) to learn syntctical rules that comprise reading difficulty in a text.
These rules are used to create a heuristic that will be used in an A* search algorithm for text generation at target reading levels. The text is then realised using [SimpleNLG](https://github.com/simplenlg/simplenlg).


### Papers published on this research
1. [Access to multimodal articles for individuals with sight impairments.](https://dl.acm.org/doi/10.1145/2395123.2395126)
2. [Enabling text readability awareness during the micro planning phase of NLG applications.](https://www.aclweb.org/anthology/W16-6621/)
3. [Evaluating the accessibility of line graphs through textual summaries for visually impaired users.](https://dl.acm.org/doi/10.1145/2661334.2661368)
4. [Adapting Graph Summaries to the Users’ Reading Levels.](https://www.aclweb.org/anthology/W14-4409/)
5. [Generating Summaries of Line Graphs.](http://m-mitchell.com/inlg2014/pdf/W14-4413.pdf)
6. [Providing Access to the High-level Content of Line Graphs from Online Popular Media.](https://dl.acm.org/doi/pdf/10.1145/2461121.2461123)
7. [Team UDEL KBGen 2013 Challenge.](https://www.aclweb.org/anthology/W13-2132.pdf)
