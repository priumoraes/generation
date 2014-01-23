#pragma once
#include <stdio.h>
#include <time.h>
#include <string>
#include <sstream>
#include "Input.h"
//#include "PageRank.h"

using namespace std;

struct dateDetail{
	string initialYear;
	string endYear;
	string initialMonth;
	string endMonth;
	string initialDay;
	string endDay;
	int numberYears;
	int numberMonths;
	int numberDays;
};


class Helper
{

public:
	Helper::Helper(void);
	string Helper::getCurrentDate();
	string Helper::getTrendDate(simpleTrend aTrend, int dateOrder);
	string Helper::getTrendDateHelper(trendDate aDate);
	string Helper::getTrendName(string trendName);
	string Helper::getSuggestionName(string category);
	int Helper::getGraphNumberYears();
	int Helper::getGraphNumberMonths();
	int Helper::getGraphNumberDays();
	dateDetail Helper::getPeriods(string aDate1, string aDate2);
	//dateDetail Helper::getPeriods(string aDate1, string aDate2, string initialYear, string endYear, string initialMonth, string endMonth, string initialDay, string endDay);
	Helper::~Helper(void);

};