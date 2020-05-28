/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * Utility class for providing a comparator.
 */
package edu.udel.eecis.teahouse.sight;

import java.util.Comparator;
import java.util.Map;

class ValueComparator implements Comparator<String> {

    Map<String, Integer> base;
    public ValueComparator(Map<String, Integer> foundSynonyms) {
        this.base = foundSynonyms;
    }

    // Note: this comparator imposes orderings that are inconsistent with equals.    
    public int compare(String a, String b) {
        if (base.get(a) >= base.get(b)) {
            return -1;
        } else {
            return 1;
        } // returning 0 would merge keys
    }
}