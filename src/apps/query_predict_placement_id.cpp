/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "../starspace.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <boost/algorithm/string/predicate.hpp>

using namespace std;
using namespace starspace;

int main(int argc, char** argv) {
  shared_ptr<Args> args = make_shared<Args>();
  if (argc < 6) {
    cerr << "usage: " << argv[0] << " <model> k [basedoc] [basedoc_labels] #examples\n";
    return 1;
  }
  std::string model(argv[1]);
  args->K = atoi(argv[2]);
  args->model = model;
  args->fileFormat = "labelDoc";
  args->basedoc = argv[3];

  const size_t SIZE = atoi(argv[5]);

  string labels[SIZE];

  ifstream labels_file(argv[4]);

  size_t i = 0;
  while(!labels_file.eof() && i < SIZE) {
    getline(labels_file,labels[i]); // read the next line into the next string
    ++i;
  } 

  StarSpace sp(args);
  if (boost::algorithm::ends_with(args->model, ".tsv")) {
    sp.initFromTsv(args->model);
  } else {
    sp.initFromSavedModel(args->model);
    cout << "------Loaded model args:\n";
    args->printArgs();
  }
  // Set dropout probability to 0 in test case.
  sp.args_->dropoutLHS = 0.0;
  sp.args_->dropoutRHS = 0.0;
  // Load basedocs which are set of possible things to predict.
  sp.loadBaseDocs();
  for(;;) {
    string input;
    // cout << "\n";
    cout << "Enter some text: ";
    if (!getline(cin, input) || input.size() == 0) break;
    // Do the prediction
    vector<Base> query_vec;
    sp.parseDoc(input, query_vec, " ");
    vector<Predictions> predictions;
    sp.predictOne(query_vec, predictions);
    // for (int i = 0; i < predictions.size(); i++) {
    //   cout << predictions[i].second << " ";
    //   //std::string docid = std::to_string(predictions[i].second);
    //   cout << predictions[i].first;
    //   cout << "\n";
    // }
    // cout << "__end__" << input << "\n";
    for (int i = 0; i < predictions.size(); i++) {
      cout << i << "[" << predictions[i].first << "]: ";
      cout << labels[predictions[i].second] << "\n";
    }
    cout << "\n";
  }
  
  return 0;
}