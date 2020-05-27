#pragma once
#include   <stdio.h>
#include   <string.h>
#define HZ2PY_OUTPUT_BUF_ARRAY_SIZE 5120
#define HZ2PY_MAX_PINYIN_SIZE 30			
#define HZ2PY_UTF8_CHECK_LENGTH 20			

bool safeAddToOutbuf(char* outbuf, int& iOutbuf, char* pinyinValue, int iPinyinValue) {
	int iOutbufWord = 1, iPinyinValueWord = 1, m;
	for (m = 0; m < iOutbuf; m++) {	
		if (outbuf[m] == ' ') {
			iOutbufWord++;
		}
	}
	for (m = 0; m < iPinyinValue; m++) {	
		if (pinyinValue[m] == '|') {
			iPinyinValueWord++;
		}
	}
	bool flag = false;	
	int iNewOutbuf = iOutbufWord * iPinyinValue + iPinyinValueWord * iOutbuf + iOutbufWord + iPinyinValueWord - 1 - iOutbufWord * iPinyinValueWord;
	if (iNewOutbuf < HZ2PY_OUTPUT_BUF_ARRAY_SIZE) {
		int totalWord = iOutbufWord * iPinyinValueWord;
		char** tempOutbuf = new char* [totalWord];
		if (!tempOutbuf) {
			fprintf(stdout, "FATAL ERROR: out of memory (failed to malloc %d bytes)\n", totalWord);
			return false;
		}
		for (m = 0; m < totalWord; m++) {
			tempOutbuf[m] = new char[HZ2PY_OUTPUT_BUF_ARRAY_SIZE];
			if (!tempOutbuf[m]) {
				fprintf(stdout, "FATAL ERROR: out of memory (failed to malloc %d bytes)\n", HZ2PY_OUTPUT_BUF_ARRAY_SIZE);
				while (m > 0) {
					delete[] tempOutbuf[m - 1];
					m--;
				}
				delete[] tempOutbuf;
				return false;
			}
		}
		char* tmp1 = outbuf;
		int n = 0, i, j, k;
		while (*tmp1 != '\0' || n == 0)	
		{
			n += iPinyinValueWord;
			i = 0;
			while (*tmp1 != ' ' && *tmp1 != '\0')
			{
				for (m = n - iPinyinValueWord; m < n; m++)
				{
					tempOutbuf[m][i] = *tmp1;
				}
				i++;
				tmp1++;
			}
			if (*tmp1 == ' ')
			{
				tmp1++;
			}
			k = 0;
			for (m = n - iPinyinValueWord; m < n; m++)
			{
				j = i;
				while (k < iPinyinValue && pinyinValue[k] != '|' && pinyinValue[k] != '\0') {
					tempOutbuf[m][j] = pinyinValue[k];
					j++;
					k++;
				}
				if (pinyinValue[k] == '|')
				{
					k++;
				}
				tempOutbuf[m][j] = '\0';
			}
		}


		outbuf[0] = '\0';
		for (m = 0; m < totalWord; m++)
		{
			outbuf = strcat(outbuf, tempOutbuf[m]);

			if (m != totalWord - 1)
			{
				outbuf = strcat(outbuf, " ");
			}
		}
		iOutbuf = iNewOutbuf;

		for (m = 0; m < totalWord; m++)
		{
			delete[] tempOutbuf[m];
		}
		delete[] tempOutbuf;
		flag = true;
	}
	return flag;
}

const char* _pinyin_table_[20902] = {
	"yi", "ding|zheng", "yu", "qi", "shang", "xia", "myeon", "wan|mo", "zhang", "san", "shang", \
	"xia", "qi", "bu|dun|fou|fu", "yu", "mian", "gai", "chou", "chou", "zhuan", "qie|ju", \
	"pi", "shi", "shi", "qiu", "bing", "ye", "cong", "dong", "si", "cheng|sheng|zheng", \
	"diu", "qiu", "liang", "diu", "you", "liang", "yan", "ban|bang|bing", "sang", "shu", \
	"jiu", "gan|ge", "ya", "pan", "zhong", "ji", "jie", "feng", "guan|kuang", "chuan|guan", \
	"chan", "lin", "zhuo", "zhu", "ha", "wan", "dan", "wei", "zhu", "dan|jing", \
	"li", "ju", "pianpang|pie|yi", "fu", "yi", "ai|yi", "ai|nai", "wu", "jiu", "jiu", \
	"zhe|tuo", "me|mo|yao|ma", "xi|yi", "ho", "zhi", "wu", "zha|zuo", "hu", "fa", "le|yue", \
	"pan|yin", "ping", "pang", "jiao|qiao", "hu", "guai", "cheng|sheng", "cheng|sheng", "jue|yi", "yin", \
	"wan|ya", "mie|nie", "jiu", "qi", "ye|yi", "xi", "xiang", "gai", "jiu", "hal", \
	"hol", "shu", "dou|dul", "shi", "ji", "keg|nang", "kal", "keol", "tol", "mol", \
	"ol", "mai", "luan", "cal", "ru", "xue", "yan", "phoi", "sal|sha", "na", \
	"gan|qian", "sol", "eol", "cui", "ceor", "qian|gan", "zhi|luan", "gui", "gan", "luan", \
	"lin", "yi", "jue", "le|liao", "ma", "yu|zhu", "zheng", "shi|zhi", "shi|zi", "er", \
	"chu", "xu|yu", "kui|yu", "xu|yu", "yun", "hu", "qi", "wu", "jing", "si", \
	"sui", "gen|xuan", "geng|gen", "e|ya", "xie|suo", "e|ya", "zhai|qi", "e|ya", "ji|qi", "wen", \
	"wang|wu", "gang|geng|kang", "da", "jiao", "hai|jie", "yi", "chan", "heng|peng|xiang", "mu", "ye", \
	"xiang", "jing", "ting", "liang", "xiang", "jing", "ye", "qin|qing", "bo", "you", \
	"xie", "chan|dan|zhan", "lian", "duo", "men|wei", "ren", "ren", "ji", "ra", "wang|wu", \
	"yi", "shen|shi", "ren", "le|li", "ding", "ze", "fu|jin|nu", "pu", "chou|qiu", "ba", \
	"zhang", "jin", "ge|jie", "bing", "reng", "cong", "fo", "san|jin|tao", "lun", "e|bing", \
	"cang|chuang", "zai|zi", "shi", "ta|tuo", "zhang", "fu", "xian", "xian", "tuo|duo|cha", "hong", \
	"tong", "ren", "qian", "gan|han", "yi|ge", "bo", "dai", "lian|ling", "si|yi", "chao", \
	"chang|zhang", "sa", "shang", "yi", "mu", "men", "ren", "fan", "chao|miao", "ang|yang", \
	"qian|jing", "zhong", "bi|pi", "wo", "wu", "jian|mou", "jia|jie", "yao|fo", "feng", "cang|chuang", \
	"lin|ren", "wang", "bin|fen", "di", "fang|pang", "zhong", "qi", "pei", "xu|yu", "diao", \
	"dun", "wu|wen", "yi", "lin|xin", "gang|kang", "yi", "fan|ji", "ai", "wu", "ji|qi", \
	"fu", "fa", "xiu|xu", "jin|yin", "pi", "dan|shen", "fu", "nu|tang", "yin|zhong", "you", \
	"huo", "hui|kuai", "yu", "cui|zu", "yun", "san", "wei", "chuan|zhuan", "che|ju", "ya", \
	"qian|xian", "shang", "chang|cheng|zheng", "lun", "cang|chen", "xun", "xin", "e|gui|wei", "zhu", "chi|che", \
	"xuan|xian", "nu", "bo|bai", "gu", "ni", "ni", "xie", "ban|pan", "xu", "ling", \
	"zhou", "shen", "qu|zu", "ci|si", "peng|beng", "si|shi", "jia|ga|qie", "pi", "zhi|yi", "shi|si", \
	"yi|si|chi", "zheng", "dian|tian", "gan|han", "mai", "dan|tan|yan", "zhu", "bu", "qia|qu", "bi", \
	"zhao|shao", "ci", "li|wei", "di", "zhu", "zuo", "you", "yang", "ben|cui|ti", "zhan|dian|chan", \
	"he", "bi", "tuo|yi", "she", "tu|xu|yu", "yi|die", "fo|fu", "zuo", "gou|kou", "ning", \
	"tong", "ni", "xian", "qu", "yong", "wa", "qian", "shi", "ka", "bao", \
	"pei", "huai|hui", "ge|he", "lao|liao", "xiang", "ge|e", "yang", "bai|mo", "fa", "ming", \
	"jia", "er|nai", "bing", "ji", "hen|heng", "huo", "gui", "quan", "diao|tiao", "jiao|xiao", \
	"ci", "yi", "shi", "xing", "shen", "tuo", "kan", "zhi", "hai|gai", "lai", \
	"yi", "chi", "e|hua|kua|wu", "guang|gong", "li|lie", "yin", "shi", "mi", "zhou|zhu", "xu", \
	"you", "an", "lu", "mao|mou", "er", "lun", "tong|dong", "cha", "chi", "xun", \
	"gong", "zhou", "yi", "ru", "cun|jian", "xia", "si", "dai", "lv", "ta", \
	"jiao|yao", "zhen", "ce|zhai", "jiao|qiao", "kuai", "chai", "ning", "nong", "jin", "wu", \
	"hou", "jiong", "cheng|ting", "chen|zhen", "zuo", "chou", "qin", "lv", "ju", "dou|shu", \
	"ting", "shen", "tuo|tui", "bo", "man|nan", "xiao", "bian|pian", "tui", "yu", "xi", \
	"chuo|cu", "e", "qiu", "xu|shu", "guang", "ku", "wu", "dun|jun|shun", "yi", "fu", \
	"liang|lang", "zu", "qiao|xiao", "li", "yong", "hun", "jing|ying", "xian|qian", "san", "pei", \
	"su", "fu", "xi", "li", "mian|fu", "ping", "bao", "yu|shu", "si|qi", "xia", \
	"shen|xin", "xiu", "yu", "di", "che|ju", "chou|dao", "zhi", "yan", "lia|liang", "li", \
	"lai", "si", "jian", "xiu", "fu", "huo", "ju", "xiao", "pai", "jian", \
	"biao", "shu|chu|ti", "fei", "beng|feng", "ya", "an|yan", "bei", "yu", "xin", "bei|bi|pi", \
	"hu|chi", "chang|cheng", "zhi", "bing", "jiu", "yao", "zu|cui", "lia|liang", "wan", "lai|lie", \
	"cang|chuang", "zong", "ge", "guan", "bei|pei", "tian", "shu", "shu", "men", "dao", \
	"dan|tan", "jue", "chui|zhui", "xing", "peng|ping", "tang|chang", "hou", "ji|yi", "qi", "diao|ti|zhou", \
	"gan", "jing|liang", "jie", "sui", "chang", "jie|qie", "fang", "zhi", "kong", "juan", \
	"zong", "ju", "qian|qing", "ni|nie", "lun", "zhuo", "wei|wo", "luo", "song", "ling|leng", \
	"hun", "dong", "zi", "ben", "wu", "ju", "nai", "cai", "jian", "zhai", \
	"ya|ye", "zhi", "sha", "qing", "qie", "ying", "chen|cheng", "qian|jian", "yan", "ruan|ru", \
	"zhong|tong", "chun", "jia|jie|xia", "jie|ji", "wei", "yu", "bing", "re|ruo", "ti", "wei", \
	"pian", "yan", "feng", "tang|dang", "wo", "e", "xie", "che", "sheng", "kan", \
	"di", "zuo", "cha", "ting", "bei", "ye|xie", "huang", "yao", "zhan", "chou|qiao", \
	"yan|an", "you", "jian", "xu", "zha", "ci", "fu", "fu|bi", "zhi", "cong|zong", \
	"mian", "ji", "yi", "xie", "xun", "si|cai", "duan", "ce|ze|zhai", "zhen", "ou", \
	"tou", "tou", "bei", "zan|za", "lou|lv", "jie|qie", "e|gui|wei", "fen", "chang", "kui|gui", \
	"sou", "si|zhi", "su", "xia", "fu", "yuan", "rong", "li", "nu", "yun", \
	"gou|jiang", "ma", "bang|beng|pang|peng", "dian", "tang", "hao", "jie", "xi", "shan", "qian|jian", \
	"que|jue", "cang|chen", "chu", "san", "bei", "xiao", "rong|yong", "yao", "ta|tan", "suo", \
	"yang", "fa", "bing", "jia|jie", "dai", "zai", "tang", "gu", "bin", "chu", \
	"nuo", "can|san", "lei", "cui", "yong|chong", "cao|zao", "zong", "beng|peng", "song|shuang", "ao", \
	"chuan|zhuan", "yu", "zhai", "qi|zu|cou", "shang", "chuang", "jing", "chi", "sha", "han", \
	"zhang", "qing", "yan|yin", "di", "su|xie", "lou|lv", "bei", "biao|piao", "jin", "lian", \
	"liao|lu", "man", "qian", "xian", "tan|lan", "ying", "dong", "zhuan|zun", "xiang", "shan", \
	"jiao|qiao", "jiong", "tui", "cuan|zun", "bu|pu", "xi", "lao", "chang", "guang", "lao|liao", \
	"qi", "deng|cheng|teng", "chan|zhuan", "e|gui|wei", "ji", "bo", "hui", "chun|chuan", "tie|jian", "dan|chan", \
	"jiao|yao", "jiu", "ceng|seng", "fen", "xian", "ju|yu", "e", "jiao", "jian|zen", "tong|zhuang", \
	"lin", "bo", "gu", "xian", "su", "xian", "jiang", "min", "ye", "jin", \
	"jia|qia", "qiao", "pi", "feng", "zhou", "ai", "sai", "yi", "jun", "nong", \
	"tan|chan|shan", "yi", "dang", "jing", "xuan", "kuai", "jian", "chu", "dan|shan", "jiao", \
	"sha", "zai", "can", "bin", "an", "ru", "tai", "chou|dao", "chai", "lan", \
	"yi|ni", "jin", "qian", "meng", "wu", "ning", "qiong", "ni", "chang", "la|lie", \
	"lei", "lv", "kuang", "bao", "yu|di", "biao", "zan", "zhi", "si", "you", \
	"hao", "qing|jing", "qin|chen", "li", "teng", "wei", "long", "chu", "chan", "rang|xiang", \
	"tiao|shu", "xie|hui", "li", "luo", "zan", "nuo", "chang|tang", "yan", "lei|luo", "nang", \
	"er|ren", "wu", "yuan|yun", "zan", "yuan", "kuang|xiong", "chong", "zhao", "xiong", "xian", \
	"guang", "dui|yue|rui", "ke", "dui|yue|rui", "mian|wan|wen", "chan|tu", "chang|zhang", "er|ni", "dui|duo|rui", "er|ni", \
	"jin", "chan|tu", "si", "yan", "yan", "shi", "shike", "dang", "qianke", "dou", \
	"gongfen", "haoke", "shen", "dou", "baike", "jing", "gongli", "huang", "ru", "wu|wang", \
	"nei|na", "quan", "liang", "yu|shu", "ba", "gong", "liu|lu", "xi", "han|jie", "lan", \
	"gong", "tian", "guan|wan", "xin|xing", "bing", "qi|ji", "ju", "dian|tian", "zi|ci", "ppun", \
	"yang", "jian", "shou", "ji", "yi", "ji", "chan", "jiong|tong", "mao", "dan|ran", \
	"na|nei|rui", "yan", "mao", "gang", "dan|ran", "ce|zha", "jiong", "ce|zha", "zai", "gua", \
	"jiong", "mao|mo", "zhou", "mao|mo", "gou", "xu", "mian", "tu", "rong", "you|yin", \
	"xie", "hem|kan", "jun", "nong", "yi", "mi|shen", "shi", "guan", "meng", "zhong", \
	"ju|zui", "yuan", "mian|ming", "kou", "lem|min", "fu", "xie", "mi", "liang", "dong", \
	"tai", "gang", "feng|ping", "bing|ning", "hu", "chong", "jue|que|xue", "ya|hu", "kuang", "ye", \
	"leng|ling", "pan", "fa|fu", "min", "dong", "sheng|xian", "lie", "qia", "jian", "cheng|jing", \
	"sou", "mei", "tu", "qi|qian", "gu", "zhun", "song", "jing|cheng", "liang", "qing", \
	"diao", "ling", "dong", "gan", "jian", "yin", "cou", "ai", "li", "cang", \
	"ming", "zhun", "cui", "si", "duo", "jin", "lin", "lin", "ning", "xi", \
	"du|dou", "ji", "fan", "fan", "fan", "feng", "ju", "chu", "zheng", "feng", \
	"mu", "zhi", "fu", "feng", "ping", "feng", "kai", "huang", "kai", "gan", \
	"deng", "ping", "qian", "xiong", "kuai", "tu", "ao|wa", "chu", "ji", "dang", \
	"han", "han", "zao|zuo", "dao|diao", "diao", "li", "ren", "ren", "chuang", "ban|fen", \
	"qi|qie", "yi", "ji", "kan", "qian", "cun", "chu", "wen", "ji", "dan", \
	"xing", "hua|huai", "wan", "jue", "li", "yue", "li|lie", "liu", "ze", "gang", \
	"chuang", "fu", "chu", "qu", "diao", "shan", "min", "ling", "zhong", "pan", \
	"bie", "jie", "jie", "pao|bao", "li", "shan", "bie", "chan", "jing", "gua", \
	"geng", "dao", "chuang", "kui", "ku|kuo", "duo", "er", "zhi", "shua", "quan|xuan", \
	"sha|cha", "ci|qi", "ke|kei", "jie", "gui", "ci", "gui", "ai|kai", "duo", "ji", \
	"ti", "jing", "dou", "luo", "ze", "yuan", "cuo", "xiao|xue", "ke|kei", "la", \
	"jian|qian", "sha|cha", "chuang", "gua", "jian", "cuo", "li", "ti", "fei", "pou", \
	"chan", "qi", "chuang", "zi", "gang", "wan", "bo|bao|pu", "ji", "duo|chi", "qing|lue", \
	"yan|shan", "zhuo|du", "jian", "ji", "bo|bao", "yan", "ju", "huo", "sheng", "jian", \
	"du|duo", "duan|tuan|zhi", "wu", "gua", "fu|pi", "sheng", "jian", "ge", "da|zha", "ai|kai", \
	"chuang|qiang", "chuan", "chan", "zhuan|tuan", "lu|jiu", "li", "peng", "shan", "biao|piao", "kou", \
	"jiao|chao", "gua", "qiao", "jue", "hua", "zha", "zhu|zhuo", "lian", "ju", "pi", \
	"liu", "gui", "chao|jiao", "gui", "jian", "jian", "tang|tong", "hua|huo", "ji", "jian", \
	"yi", "jian", "zhi", "chan", "zuan", "mi|mo", "li", "zhu", "li", "ya", \
	"quan", "ban", "gong", "jia", "wu", "mai", "lie", "jin|jing", "keng", "xie|lie", \
	"zhi", "dong", "chu|zhu", "nu", "jie", "qu", "shao", "yi", "zhu", "miao|mo", \
	"li", "jin|jing", "lao|liao", "lao", "juan", "kou", "gu|yang", "wa", "xiao", "mou", \
	"kuang", "jie", "lie", "he|kai", "shi", "ke|kei", "jin|jing", "gao", "bo", "min", \
	"chi", "lang", "yong", "yong", "mian", "ke|kei", "xun", "juan", "qing", "lu", \
	"bu", "meng", "chi|lai", "le|lei", "kai", "mian", "dong", "mao|xu", "xu", "kan", \
	"mao|wu", "yi", "xun", "weng|yang", "sheng", "lao|liao", "bo|mu", "lu", "piao", "shi", \
	"ji", "qi|qin", "jiang|qiang", "jiao|chao", "quan", "xiang", "yi", "qiao|jue", "fan", "juan", \
	"tong|dong", "ju", "dan", "xie", "mai", "xun", "xun", "lv", "li", "che", \
	"rang|xiang", "quan", "bao|pianpang", "di|shao|zhuo", "jun|yun", "jiu", "bao", "gou", "mo|wu", "jun|yun", \
	"mangmi", "bi", "gai", "gai", "bao|fu|pao", "cong", "yi", "xiong", "peng", "ju", \
	"tao|yao", "ge", "pu", "e", "pao", "fu", "gong", "da", "jiu", "gong", \
	"bi|pin", "hua|huo", "bei", "nao", "shi|chi", "fang|pianpang", "jiu", "yi", "za", "jiang", \
	"kang", "jiang", "kuang|wang", "hu", "xia", "qu", "fan", "gui", "qie", "zang|cang", \
	"kuang", "fei|fen", "hu", "yu", "gui", "kui|gui", "hui", "dan", "kui|gui", "lian", \
	"lian", "suan", "du", "jiu", "qu|jue", "xi", "pi", "ou|qu", "yi", "an|e|ke", \
	"yan", "bian", "ni|te", "ou|qu", "shi", "xun", "qian", "nian", "sa", "cu|zu", \
	"sheng", "wu", "hui", "ban|pan", "shi", "xi", "wan", "hua", "xie", "wan", \
	"bei|bi|pi", "zu|cu", "zhuo", "xie", "shan|dan|chan", "mai", "nan|na", "chan|dan|shan", "chi|ji", "bo", \
	"shuai|lv", "bu|bo", "kuang|guan", "bian|pan", "bu|ji", "tie|zhan", "ka|qia", "lei|lu|lv", "you", "lu", \
	"xi", "gua", "wo", "xie", "dan|jie", "jie|ran", "wei", "ang|yang", "qiong", "zhi", \
	"mao", "yi|yin", "wei", "shao", "ji", "jiao|que", "kun|luan", "chi", "juan|quan", "xie", \
	"xu|su", "jin", "jiao|que|xi", "kui", "ji", "e", "qing", "xi", "san", "an|chang|yan", \
	"yan", "e", "ting", "li", "zhai|zhe", "an|han", "li", "ya", "ya", "ya|yan", \
	"she", "di", "zha|zhai", "long|pang", "ting|a", "qie", "ya", "zhi|shi", "ce|si", "mang", \
	"ti", "chan|li", "she", "hou", "ting", "zui", "cuo|ji", "fei", "yuan", "ce|ci|si", \
	"yuan", "xiang", "yan", "li", "jue", "xia|sha", "dian", "chu", "jiu", "jin|qin", \
	"ao", "gui", "yan|ya", "si", "li", "chang", "qian|lan", "lai|li", "yan", "yan", \
	"yuan", "si", "hong|gong", "miao|lin|mian", "rou|qiu", "qu", "qu", "keum", "lei", "du", \
	"xian", "zhuan|hui", "san", "can|shen|cen", "can|shen|cen|san", "can|shen|cen|san", "can|shen|cen|san", "ai|yi", "dai", "you", \
	"cha|chai", "ji", "you", "shuang", "fan", "shou", "guai|jue", "ba", "fa", "ruo", \
	"shi|li", "shu", "zhuo|jue|yi|li", "qu", "dao|shou", "bian", "xu", "jia|xia", "pan", "sou|xiao", \
	"ji", "yu|wei", "sou|xiao", "die", "rui", "cong", "kou", "gu|ku", "ju|gou", "ling", \
	"gua", "dao|tao", "kou", "zhi", "jiao", "zhao|shao", "ba|pa", "ding", "ge|ke", "si|tai|yi", \
	"chi", "shi", "you", "qiu", "po", "ye|xie", "hao|xiao", "ci|si", "tan|yi|you", "chi", \
	"le|li", "diao", "ji", "dug", "hong", "mie", "xu|yu", "mang", "chi|qi", "ge", \
	"song|xuan", "yao", "ji|zi", "he|ge", "ji", "diao", "cun|duo|dou", "tong", "ming", "hou", \
	"li", "tu", "xiang", "zha", "xia|he", "ye", "lv", "ya|a", "ma", "ou", \
	"huo", "yi|xi", "jun", "chou", "lin", "tian|tun", "jin|yin", "fei", "bi|pi", "qin", \
	"qin", "jie|ge", "bu|pou", "fou|pi", "ba|pa", "dun|tun", "fen|pen", "hua|e", "han", "ting|yin", \
	"hang|keng", "shun", "qi", "hong", "zhi|zi", "yin|shen", "wu|yu", "tun|wu", "chao|miao", "na|ne", \
	"jue|xue|chuo", "xi", "chui", "dou|ru", "wen", "hou", "hong|ou|hou", "wu|ya|yu", "gao", "xia|ya", \
	"jun", "lv", "ai|e", "ge", "wen|mei", "dai|ai", "qi", "cheng", "wu", "gao|gu|ju", \
	"fu", "jiao", "yun", "chi", "sheng", "na|ne", "tun|tian", "fu|mu", "yi", "tai|dai", \
	"ou|xu", "li", "bei|bai", "yuan|yun", "wa|wo|gua|guo|he|wai", "wen|hua|qi", "cheng|qiang", "wu", "e|ai", "shi", \
	"juan", "pen", "min|wen", "ne|ni", "mu|mou", "ling", "ran", "you", "di", "zhou", \
	"shi", "zhou", "tie|che", "xi|chi", "yi", "zhi|qi", "ping", "ci|zi", "gua|gu", "ci|zi", \
	"mei|wei", "hou|gou|xu", "he|ke", "na|nao|nu", "xia|ga", "pei", "chi|yi", "hao|xiao", "shen", "he|hu|xiao|xu", \
	"ming", "dan|da|ya", "qu|ka", "ju|zui", "gan|han|xian", "za", "tuo", "duo", "pou", "pao", \
	"bi|bie", "fu", "yang", "he|huo", "za|zha|ze", "he|huo|hu", "tai|hai", "gao|jiu", "yong", "fu", \
	"da", "zhou", "wa", "ka|nong", "gu", "ka|ga", "zo|zuo", "bu", "long", "dong", \
	"ning", "ta|tuo", "si", "xian", "huo", "qi", "er", "e", "gong|guang", "zha", \
	"die|xi", "xi|yi", "lei|lie", "zi", "mie", "mai|mie", "zhi", "jiao|yao", "ji|xi|qia", "ru|zhou|zhu", \
	"ge|luo|lo|ka", "xun|shu", "zan|za", "xiao", "ke|hai", "hai|hui", "kua", "shi|huai", "tiao|tao", "xian", \
	"an|e", "xuan", "xiao|xiu|xu", "guo|wai", "yan|ye", "lao", "yi", "ai", "pin", "shen", \
	"tong", "hong", "hong|xiong", "duo|chi", "gui|hua|wa", "ha|he", "zai", "you", "di|mi", "gu|pai", \
	"xiang", "ai", "gen|hen", "kuang|qiang", "e|ya", "da", "xiao", "bi", "hui|yue", "nian", \
	"hua", "xing", "guai|kuai|kuo|wei", "duo", "ppun", "ji|zhai|jie", "nang|nong", "mou", "yo", "hao", \
	"yuan|yun", "long|ka", "tou|pou", "mang", "ge", "o|e", "chi|xia", "sao|shao|xiao", "li|mai", "na|ne", \
	"zu", "he", "ku", "xiao|xue", "xian", "lao", "bo|po", "zhe", "zha", "liang|lang", \
	"ba", "mie", "lv|lie", "sui", "fu", "bu|fu", "han", "heng", "geng|ying", "shuo|yue", \
	"jia|ge", "you", "yan", "gu", "gu", "bei|bai", "han", "shua|suo", "chun|zhen", "yi", \
	"ai", "jia|qian", "tu", "dan|xian|yan", "wan", "li", "xi|xie", "tang", "shi|zuo", "qiu", \
	"che", "wu", "zao", "ya|e", "dou", "qi", "di", "qin", "mai|ma", "mas", \
	"gong|hong", "teo", "keos", "chao|lao|xiao", "liang", "suo", "zao", "huan", "lang", "sha", \
	"ji|jie", "zo", "wo|wei", "beng|feng", "jin|yin", "hu|xia", "qi", "shou|shu", "wei", "shua", \
	"chang", "er|wa", "li", "qiang", "an", "ze|zuo|jie", "yo|yu", "nian|dian", "yu", "tian", \
	"lai", "qie|sha", "xi", "tuo", "hu", "ai", "zhou|zhao|tiao", "nou|gou", "ken", "zhou|zhuo", \
	"zhuo|zhao", "shang", "di|shi", "heng|e", "lin|lan", "a", "cai|xiao", "qiang|xiang", "tun|zhun|dui", "wu", \
	"wen", "cui|qi", "jie|die|ti|sha", "gu", "qi", "qi", "tao", "dan", "dan", "yue|wa|ye", \
	"ci|zi", "bi|tu", "cui", "chuo|chuai", "he", "ya|e", "qi", "zhe", "fei|pai|pei", "liang", \
	"xian", "pi", "sha", "la", "ze", "qing|ying", "gua", "pa", "ze|shi", "se", \
	"zhuan", "nie|yao", "guo", "luo", "ngam", "di", "quan|jue", "chan|tan|tuo", "bo", "ding", \
	"lang", "chi|xiao", "geu", "tang", "chi|di", "ti", "an", "jiu", "dan", "ka|ke", \
	"yu|yong", "wei", "nan", "shan", "yu", "zhe", "la", "jie|xie", "hou", "han|jian|kan", \
	"die|zha", "zhou", "chai", "wai", "nuo|re", "yu|xu|guo|huo", "yin", "zan|za", "yao", "wo|o", \
	"mian", "hu", "yun", "chuan", "hui|zhou", "huan", "yuan|huan|xuan|he", "chi|xi", "he|ye", "ji", \
	"huai|kui", "zhong|chong|chuang", "wei", "che|sha", "xu", "huang", "duo|zha", "yan|nie", "xuan", "liang", \
	"yu", "sang", "chi|kai", "qiao|jiao", "yan", "dan|shan|chan", "ben|pen", "can|qi|sun", "li", "yo", \
	"zha|cha", "wei", "miao", "ying", "fen|pen", "phos", "kui", "bei", "yu", "gib", \
	"lou", "ku", "zao|qiao", "gu|hu", "ti", "yao", "he|xiao|hu", "sha|a", "xiu|xu", "cheng|qiang", \
	"se", "yong", "su", "gong|hong", "xie", "yi|ai", "shuo|suo", "ma", "cha", "hai", \
	"he|ke|xia", "da|ta", "sang", "chen|tian", "ru", "sou|su", "gu|wa", "ji", "pang|beng", "wu", \
	"qian|xian|qie", "shi", "ge", "zi", "jie|jue", "lao|lu", "weng", "wa", "si", "chi", \
	"hao", "suo", "jia|lun", "hai|hei", "suo", "qin", "nie|zhe", "he", "cis", "sai", \
	"en|ng", "go", "na", "dia", "ai", "qiang", "tong", "bi", "ao", "ao", \
	"lian", "zui|sui", "zhe|zhu", "mo", "shu|shuo|sou", "sou", "tan", "di|zhe", "qi|za|zu", "jiao", \
	"chong", "jiao|dao", "kai|ge", "tan", "can|shan", "cao", "jia", "ai", "xiao", "piao", \
	"lou", "ga", "jia|gu", "xiao|jiao", "hu", "hui", "guo", "ou|xu", "xian", "ze", \
	"chang", "xu|shi", "po", "de|dei", "ma", "ma", "hu", "le|lei", "du", "ga", \
	"tang", "ge|ye", "beng", "ying", "sai", "jiao", "mi", "chi|xiao", "hua", "mai", \
	"ran", "zuo|zhuai|chuai", "peng", "chao|lao|xiao", "chi|xiao", "ji", "zhu", "chao|zhao", "huai|kui", "zui", \
	"xiao", "si", "hao", "mu|fu", "liao", "qiao", "xi", "xu|chu", "tan|chan", "tan|dan", \
	"hei|mo|mu", "xun", "e|wu", "zun", "fan|bo", "chi|kai", "hui", "can", "chuang", "cu|za", \
	"dan", "yu", "tun|kuo", "ceng|cheng", "jiao|jiu", "sha|ye|yi", "xi", "qi", "hao", "lian", \
	"xu|shi", "deng", "hui", "yin", "pu", "jue", "qin", "xun", "nie|yao", "lu", \
	"si", "yan", "ying", "da", "zhan|dan", "ao|yu", "zhuo|zhou", "jin", "nang|nong", "hui|yue", \
	"xie", "qi", "e", "zao", "ai|yi", "shi", "jiao|qiao|chi", "yuan", "ai", "yong", \
	"xue|jue", "kuai|guai", "yu", "fen|pen", "dao", "ga|ge", "xin|hen", "dun", "dang", "xin", \
	"sai", "pi", "pi", "yin", "zui", "ning", "di", "lan|han", "ta", "huo|wo|o", \
	"ru", "hao", "he|xia", "yan|ye", "duo", "xiu|pi", "zhou|chou", "ji|zhai|jie", "jin", "hao", \
	"ti", "chang", "xun", "me", "cha|ca", "ti|zhi", "lu", "hui", "bao|bo|pao", "you", \
	"nie|yao", "yin", "yo|hu", "mo|mei|me", "hong", "zhe", "li", "liu", "xie|hai", "nang", \
	"xiao|ao", "mo", "yan", "li", "lu", "long", "po|mo", "dan", "chen", "pin", \
	"pi", "xiang", "huo|xue", "me", "xi", "duo", "ku", "yan", "chan", "ying", \
	"rang", "dian|di", "la", "ta", "xiao|ao", "jiao|jue", "chuo", "huan", "geo|huo", "zhuan", \
	"nie|zhe", "xiao|ao", "za|zha|ca", "li", "chan", "chai", "li", "yi", "luo", "nang", \
	"zan|za|can", "su", "heui", "zeng|zen", "jian", "yan|za|nie", "zhu", "lan", "nie", "nang", \
	"ram", "luo|lo", "guo|wei", "hui", "yin", "qiu", "si", "nin", "nan|jian", "hui", \
	"xin", "yin", "nan|nie", "qiu|tuan", "tuan", "tun|dun", "kang", "yuan", "jiong", "pian", \
	"yun", "cong|chuang", "hu", "hui", "wan|yuan", "e", "guo", "kun", "chuang|cong", "tong|wei", \
	"tu", "wei", "lun", "guo", "qun", "ri|shi", "ling", "gu", "guo", "tai", \
	"guo", "tu", "you", "guo", "yin", "hun|huan", "pu", "yu", "han", "yuan", \
	"lun", "quan|juan", "yu", "qing", "guo", "chuan|chui", "wei", "yuan", "quan|juan", "ku", \
	"pu", "yuan", "yuan", "ya", "tuan", "tu", "tu", "chan|tuan", "lue", "hui", \
	"yi", "huan|yuan", "luan", "luan", "cha|du|tu", "ya", "tu", "ting", "ku|sheng", "pu", \
	"lu", "kuai", "ju|ya", "zai", "xu|wei", "ge|yi", "yu|zhun", "wu", "gui", "pi", \
	"yi", "di|de", "qian|su", "qian|su", "chou|huai|quan|zhen", "zhuo", "dang", "qia", "xia", "shan", \
	"kuang", "chang|dang|shang", "qi|yin", "nie", "mo", "ji|jie", "jia", "zhi", "zhi", "ban", \
	"xun", "yi", "qin", "mei|fen", "jun|yun", "keng|rong", "tun|dun", "fang", "fen|ben", "ben", \
	"tan", "kan", "huai|pei|pi", "zuo", "kang|keng", "bi", "jing|xing", "lan|di", "jing", "ji", \
	"kuai|yue", "di|chi", "jing|xing", "jian", "dan|tan", "li", "ba", "wu", "fen", "zhui", \
	"po", "ban|pan", "tang", "kun", "ju|qu", "tan", "zhi|zhuo", "tuo|yi", "gan", "ping", \
	"dian|zhen", "gua", "ni", "tai", "huai|pi", "jiong|shang", "yang", "fo", "ao|you", "lu", \
	"qiu", "mei|mu", "ke", "gou", "xue", "ba", "di|chi", "che", "ling", "zhu", \
	"fu", "hu", "zhi", "chui|zhui", "la", "long", "long", "lu", "ao", "dai|tae", \
	"pao", "min", "xing", "dong|tong", "ji", "he", "lv", "ci", "chi", "lei", \
	"gai", "yin", "hou", "dui", "zhao", "fu", "guang", "yao", "duo", "duo", \
	"gui", "cha", "yang", "ken|yin", "fa", "gou", "yuan", "die", "xie", "ken|yin", \
	"jiong|shang", "shou", "e|sheng", "bing", "dian", "hong", "e|wu|ya", "kua", "da", "ka", \
	"dang", "kai", "hang", "nao", "an", "xing", "xian", "yuan|huan", "bang", "fu|pou", \
	"ba|bei", "yi", "yin", "han|an", "xu", "chui|zhui", "cen|qin", "geng", "ai|zhi", "feng|beng", \
	"fang|di", "jue|que", "yong", "jun", "jia|xia", "di", "mai|man", "lang", "juan", "cheng", \
	"yan|shan", "qin|jin", "zhe", "lie", "lie", "pu|bu", "cheng", "hua", "bu", "shi", \
	"xun", "guo|wo", "jiong", "ye", "dian|nian|nie", "di", "yu", "bu", "ya|e", "juan|quan", \
	"sui|su", "bei|bi|pi", "qing|zheng", "wan", "ju", "lun", "zheng|cheng", "kong", "shang|chong", "dong", \
	"dai", "tan", "an|yan", "cai", "chu|tou", "bang|beng", "xian|kan", "zhi", "duo", "yi|shi", \
	"zhi", "yi", "pei|pi|pou", "ji", "zhun|dui", "qi", "sao", "ju", "ni|ban", "ku", \
	"ke", "tang", "kun", "ni", "jian", "dui|zui", "jin|qin", "gang", "yu", "e|ya", \
	"peng|beng", "gu", "tu", "leng", "fang", "ya", "jian|qian", "kun", "an", "shen", \
	"duo|hui", "nao", "tu", "cheng", "yin", "hun|huan", "bi", "lian", "guo|wo", "die", \
	"zhuan", "hou", "bao|bu|pu", "bao", "yu", "di|shi|wei", "mao|wu|mou", "jie", "ruan|nuo", "ye|e|ai", \
	"geng", "chen|kan", "zong", "yu", "huang", "e", "yao", "yan", "bao|fu", "ji|ci", \
	"mei", "chang|dang|shang", "du|zhe", "tuo", "pou|yin", "feng", "zhong", "jie", "jin", "feng|heng", \
	"gang", "chun|chuan", "jian|kan", "ping", "lei", "jiang|xing", "huang", "leng", "duan", "wan", \
	"xuan", "ji|xi", "ji", "kuai", "ying", "da|ta", "cheng", "yong", "kai", "su", \
	"su", "shi", "mi", "da|ta", "weng", "cheng", "du|tu", "tang", "qiao|que", "zhong", \
	"li", "peng", "bang", "sai|se", "zang", "dui", "chen|tian|zhen", "wu", "zheng", "xun", \
	"ge", "zhen", "ai", "gong", "yan", "kan", "chen|tian|zhen", "yuan", "wen", "xie", \
	"liu", "hai", "lang", "shang|chang", "peng", "beng", "chen", "lu", "lu", "ou|qiu", \
	"jian|qian", "mei", "mo", "zhuan|tuan", "shuang", "shu", "lou", "chi", "man", "biao", \
	"jing", "ce", "shu|ye", "zhi|di", "zhang", "kan", "yong", "dian|nian", "chen", "zhi|zhuo", \
	"ji|xi", "guo", "qiang", "jin|qin", "di", "shang", "mu", "cui", "yan", "ta", \
	"zeng", "qian", "qiang", "liang", "wei", "zhui", "qiao", "ceng|zeng", "xu", "shan|chan", \
	"shan", "fa|ba|fei", "pu", "kuai|tui", "dong|tuan", "fan", "qiao|que", "mei|mo", "dun", "dun", \
	"zun|dun|cun", "di", "sheng", "duo|hui", "duo", "tan", "deng", "wu|mu", "fen", "huang", \
	"tan", "da", "ye", "zhu", "jian", "ao|yu", "qiang", "ji", "qiao|ao", "ken", \
	"yi|tu", "pi|bi", "bi", "dian", "jiang", "ye", "weng|yong", "xue|jue|bo", "dan|shan|tan", "lan", \
	"ju", "huai", "dang", "rang", "qian", "xun", "lan|xian", "xi", "he", "ai", \
	"ya", "dao", "hao", "ruan", "jin", "lei|lv", "kuang", "lu", "yan", "tan", \
	"wei", "huai|hui", "long", "long", "rui", "li", "lin", "rang", "chan", "xun", \
	"yan", "lei", "ba", "wan", "shi", "ren", "san", "zhuang", "zhuang", "qing|sheng", \
	"yi", "mai", "ke|qiao", "zhu", "zhuang", "hu", "hu", "kun", "yi|yin", "hu", \
	"xu", "kun", "shou", "mang", "dun", "shou", "yi", "zhe|zhong|dong|zhi", "ying|gu", "chu", \
	"jiang|xiang", "feng|pang", "bei", "zhai", "bian", "sui", "qun", "ling", "bi|fu", "cuo", \
	"jia|xia|yan", "xiong|xuan", "xie", "nao", "jia|xia", "kui", "xi|yi|yu", "wai", "yuan|wan", "mao|wan", \
	"su", "che|duo|zhi", "che|duo|zhi", "ye", "qing", "oes", "gou", "gou", "qi", "meng", \
	"meng", "yin", "huo", "chen", "da|dai", "ce|ze", "tian", "ta|tai", "fu", "jue|guai", \
	"wai|wo|yao", "yang|ying", "hang|ben", "gao", "shi|yi", "ben|tao", "tai", "tou", "tao|yan", "bi", \
	"yi", "kua", "jia|ga", "dui|duo", "hwa", "kuang", "yun", "jia|ga", "ba", "en", \
	"lian", "huan", "di|ti", "yan", "pao", "juan", "qi|ji", "nai", "feng", "xie|lie", \
	"fen|kang", "dian", "juan|quan", "kui", "cou|zou", "huan", "qi|xie", "kai", "she|zha", "ben|fen", \
	"yi", "jiang", "tao", "zang|zhuang", "ben", "xi", "huang", "fei", "diao", "xun", \
	"beng|keng", "dian|ding|ting|zun", "ao", "she", "weng", "ha|tai|po", "ao|you|yu", "wu", "ao", "jiang", \
	"lian", "duo|dui", "yun", "jiang", "shi", "fen", "huo", "bi", "luan", "duo|che", \
	"nv|ru", "nu", "ding|ting", "nai", "qian", "gan|jian", "chi|jie|ta", "jiu", "nuan", "cha", \
	"hao", "xian", "fan", "ji", "shuo|yue", "ru", "fei|pei", "wang", "hong", "zhuang", \
	"fu", "ma", "dan", "ren", "fu|you", "jing", "yan", "jie|ha|hai", "wen", "zhong", \
	"pa", "du", "ji", "hang|keng", "zhong", "jiao|yao", "jin|xian", "yun", "miao", "pei|pi|fou", \
	"chi", "jue|yue", "zhuang", "hao|niu", "yan", "na|nan", "xin", "fen", "bi", "yu", \
	"tuo", "feng", "yuan|wan", "fang", "wu", "kou|yu", "gui", "du", "ba|bo", "ni", \
	"chou|zhou", "zhuo", "zhao", "da", "ni|nai", "yuan", "tou", "xuan|xian|xu", "yi|zhi", "e", \
	"mei", "mo", "qi", "bi", "shen", "qie", "e", "he", "xu", "fa", \
	"zheng", "min", "ban", "mu", "fu", "ling", "zi", "zi", "shi", "ran", \
	"pan|shan|xian", "yang", "gan|man", "jie|ju|xu", "gu", "si", "sheng|xing", "wei", "ci|zi", "ju", \
	"pan|shan|xian", "pin", "ren", "tao|tiao|yao", "dong", "jiang", "shu", "ji", "gai", "xiang", \
	"hua|huo", "juan", "jiao|xiao", "gou|du", "lao|mu", "jian", "jian", "yi", "nian", "zhi", \
	"zhen", "ji|yi", "xian", "heng", "guang", "xun|jun", "kua|hu", "yan", "ming", "lie", \
	"pei", "e|ya", "you", "yan", "cha", "xian|shen", "yin", "ti|shi|ji", "gui|wa", "quan", \
	"zi", "song", "wei", "hong", "gui|wa", "lou|lv", "ya", "rao|yao", "jiao", "lian|luan", \
	"pin|ping", "dan|xian", "shao", "li", "cheng|sheng", "xie", "mang", "fu", "suo", "wu|mu", \
	"wei", "ke", "cu|chuo|lai", "cu|chuo", "ting|tian", "niang", "xing", "nan", "yu", "na|nuo", \
	"pou|bi", "sui|nei", "juan", "shen", "zhi", "han", "di", "zhuang", "e", "ping", \
	"tui", "xian|man", "mian|wan", "wu|yu", "yan", "wu", "ai|xi", "yan", "yu", "si", \
	"yu", "wa", "li", "xian", "ju", "ju|qu|shu", "zhui|shui", "qi", "xian", "zhuo", \
	"dong", "chang", "lu", "ai|e", "e", "e", "lou|lv", "mian", "cong", "pei|bu|pou", \
	"ju", "po", "cai", "ling", "wan", "biao", "xiao", "shu", "qi", "hui", \
	"fan|fu", "wo", "wo|rui", "tan", "fei", "fei", "jie|qie", "tian", "ni", "juan|quan", \
	"jing", "hun", "jing", "jin|qian", "dian", "xing", "hu", "guan|wan", "lai", "bi", \
	"yin", "chou|zhou", "chuo|nao", "fu", "jing", "lun", "an|nue", "lan", "kun|hun", "yin", \
	"ya", "ju", "li", "dian", "xian", "hua", "hua", "ying", "chan", "shen", \
	"ting", "yang|dang", "yao", "mou|mu|wu", "nan", "ruo|chuo", "jia", "tou|yu", "xu", "yu", \
	"wei", "di|ti", "rou", "mei", "dan", "ruan|nen", "qin|qing", "hui", "wo", "qian", \
	"chun", "miao", "fu", "jie", "duan", "pei|xi|yi", "zhong", "mei", "huang", "mian", \
	"an|yan", "ying", "xuan", "jie", "wei", "mei", "yuan", "zheng", "qiu", "ti|shi", \
	"xie", "duo|tuo", "lian", "mao", "ran", "si", "pian", "wei", "wa", "cu", \
	"hu", "yun|ao|wo", "jie|qie", "bao", "xu", "yu|tou", "gui", "zou|chu", "yao", "bi|pi", \
	"xi", "yuan", "sheng|ying", "rong", "ru", "chi", "liu", "mei", "pan", "yun|ao|wo", \
	"ma", "gou", "chou|kui", "qin|shen", "jia", "sao", "zhen", "yuan", "jie|suo", "rong", \
	"ming|meng", "ying|xing", "ji", "su", "niao", "xian", "tao", "bang|pang", "lang", "nao", \
	"bao", "ai", "pi", "pin", "yi", "biao|piao", "kou|yu", "lei", "xuan", "man|yuan", \
	"yi", "zhang", "kang", "yong", "ni", "li", "di", "gui|zui", "yan", "jin", \
	"zhuan|tuan", "chang", "ce|ze", "han|nan", "nen", "lao", "mo", "zhe", "hu", "hu", \
	"ao", "nen|ruan", "qiang", "ma", "pie", "gu", "wu", "jiao|qiao", "tuo|duo", "zhan", \
	"miao", "xian", "xian", "mo", "liao", "lian", "hua", "gui", "deng", "zhi", \
	"xu", "yi", "hua", "xi", "kui", "rao|yao", "xi", "yan", "chan", "jiao", \
	"mei", "fan|fu", "fan", "xian|yan|jin", "yi", "hei", "jiao", "fan|fu", "shi", "bi", \
	"chan|shan", "sui", "qiang", "lian", "huan|xuan|qiong", "xin", "niao", "dong", "yi", "can", \
	"ai", "niang", "ning", "ma", "tiao|diao", "chou", "jin", "ci", "yu", "pin", \
	"rong", "ru|nou", "er|nai", "yan", "tai", "ying", "can|qian", "niao", "yue", "ying", \
	"mian", "bi", "ma|mo", "shen", "xing", "ni", "du", "liu", "yuan", "lan", \
	"yan", "shuang", "ling", "jiao", "niang|rang", "lan", "xian|qian", "ying", "shuang", "xie|hui", \
	"quan|huan", "mi", "li", "luan|lian", "yan", "shu|zhu", "lan", "zi", "jie", "jue", \
	"jue", "kong", "yun", "ma|zi", "zi", "cun", "sun|xun", "fu", "bo|bei", "zi", \
	"xiao", "shen|xin", "meng", "si", "tai", "bao", "ji", "gu", "nu", "hua|jiao|xue", \
	"you", "zhuan|ni", "hai", "luan", "sun|xun", "nao", "me|mie", "cong", "qian|wan", "shu", \
	"chan|can", "ya", "zi", "yi|ni", "fu", "zi", "li", "hua|jiao|xue", "bo", "ru", \
	"nai", "nie", "nie", "ying", "luan", "mian", "ning|zhu", "rong", "ta|tuo|yi", "gui", \
	"che|du|zhai", "kong|qiong", "yu", "shou", "an", "tu|jia", "song", "kuan|wan", "rou", "yao", \
	"hong", "yi", "jing", "zhun", "mi|fu", "zhu", "dang", "hong", "zong", "guan", \
	"zhou", "ding", "wan|yuan", "yi", "bao", "shi|zhi", "shi", "chong|long", "pan|shen", "ke|qia", \
	"xuan", "shi", "you", "huan", "yi", "tiao", "shi|xi", "xian|xiong", "gong", "cheng", \
	"jiong|qun", "gong", "xiao", "zai", "zha", "bao|shi", "hai|he", "yan", "xiao", "jia|jie", \
	"pan|shen", "chen", "rong|yong", "huang", "mi", "kou", "kuan", "bin", "su|xiu", "cai", \
	"zan", "ji", "yuan", "ji", "yin", "mi", "kou", "qing|qiu", "he", "zhen", \
	"jian", "fu", "ning", "bing", "huan", "mei", "qin", "han", "yu", "shi", \
	"ning", "jin|qin", "ning", "zhi|tian", "yu", "bao", "kuan", "ning", "qin", "mo", \
	"cha|cui", "lou|lv|ju", "gua", "qin", "hu", "wu", "liao", "shi|zhi", "ning", "qian|se|zhai", \
	"pan|shen", "wei", "xie", "kuan", "hui", "liao", "jun", "huan|xian", "yi", "yi", \
	"bao", "qin", "chong|long", "bao", "feng", "cun", "dui", "shi|si", "xun|xin", "dao", \
	"lv|luo|lue", "dui", "shou", "po", "bian|feng", "zhuan", "fu|bu", "she|ye|yi", "kei|ke", "jiang|qiang", \
	"jiang|qiang", "shuan|tuan|zhuan", "wei|yu", "zun", "xin|xun", "shu|zhu", "dui", "dao", "xiao", "jie|ji", \
	"shao", "er|mi", "er|mi", "er|mi", "ga", "jian", "shu", "chen", "chang|shang", "chang|shang", \
	"ma|mo", "ga", "chang", "liao", "xian", "xian", "kun|hun", "pianpang|wang|you", "wang|you", "you", \
	"liao|niao", "liao|niao", "yao", "long|pang|mang|meng", "wang", "wang", "wang", "ga", "yao", "duo", \
	"kui", "zhong", "jiu", "gan", "gu", "gan", "tui|zhuai", "gan", "gan", "shi", \
	"yin|yun", "chi|che", "kao", "ni", "jin", "wei|yi", "niao|sui", "ju", "pi", "ceng", \
	"xie|xi", "bi", "ji|ju", "jie", "tian", "jue|que", "ti", "jie", "wu", "diao", \
	"shi", "shi|xi", "ping|bing", "ji", "xie", "zhen", "xie|xi", "ni", "zhan", "xi", \
	"uu", "man", "e|ke", "lou", "ping|bing", "ti|xie", "fei", "shu|zhu", "xie|ti", "tu", \
	"lv", "lv", "xi", "ceng", "lv", "ju", "xie", "ju", "jue", "liao", \
	"jue", "shu|zhu", "xie|xi", "cao|che", "tun|zhun", "ni|ji", "shan", "wa", "xian", "li", \
	"yan", "dao", "hui", "hong|long", "yi|ge", "qi", "ren", "wu", "an|han", "shen", \
	"yu", "chu", "sui|suo", "qi", "yen", "yue", "ban", "yao", "ang", "xia|ya", \
	"wu", "jie", "ji|e", "ji", "qian", "fen|cha", "wan", "qi", "cen", "qian", \
	"qi", "cha", "jie", "qu", "gang", "xian", "ao", "lan", "dao", "ba", \
	"zuo", "zuo", "yang", "ju", "gang", "ke", "gou", "xue", "po", "li", \
	"tiao", "zu|ju|qu", "yan", "fu", "xiu", "jia", "ling", "tuo", "pi", "ao", \
	"dai", "kuang", "yue", "qu", "hu", "po", "min", "an", "tiao", "ling", \
	"chi", "ping", "dong", "ceom", "kui|wei", "bang", "mao", "tong", "xue", "yi", \
	"bian", "he", "ke|ba", "luo", "e", "fu|nie", "xun", "die", "lu", "en", \
	"er", "gai", "quan", "tong|dong", "yi", "mu", "shi", "an", "wei", "huan", \
	"zhi|shi", "mi", "li", "ji", "tong|dong", "wei", "you", "gu", "xia", "lie", \
	"yao", "qiao|jiao", "zheng", "luan", "jiao", "e", "e", "yu", "xie|ye", "bu", \
	"qiao", "qun", "feng", "feng", "nao", "li", "you", "xian", "rong", "dao", \
	"shen", "cheng", "tu", "geng", "jun", "gao", "xia", "yin", "wu|yu", "lang|nang", \
	"kan", "lao", "lai", "xian|yan", "que", "kong", "chong", "chong", "ta", "lin", \
	"hua", "ju", "lai", "qi|yi", "min", "kun", "kun", "cui|zu", "gu", "cui", \
	"ya", "ya", "gang|bang", "lun", "lun", "ling", "jue|yu", "duo", "zheng", "guo", \
	"yin", "dong", "han", "zheng", "wei", "xiao", "bi|pi", "yan", "song", "jie", \
	"beng", "cui|zu", "jue|ku", "dong", "chan|zhan", "gu", "yin", "zi", "ze", "huang", \
	"yu", "wei|wai", "yang|dang", "feng", "qiu", "yang", "ti", "yi", "zhi|shi", "shi|die", \
	"zai", "yao", "e", "zhu", "kan|zhan", "lv", "yan", "mei", "han", "ji", \
	"ji", "huan", "ting", "cheng|sheng", "mei", "qian|kan", "mao|wu", "yu", "zong", "lan", \
	"jie|ke", "nie|yan", "yan", "wei", "zong", "cha", "sui|suo", "rong", "ke", "qin", \
	"yu", "qi|ti", "lou", "tu", "cui|dui", "xi", "weng", "cang", "tang|dang", "ying|rong", \
	"jie", "kai|ai", "liu", "wu", "song", "kao|qiao", "zi", "wei", "beng", "dian", \
	"ci|cuo", "qian|qin", "yong", "nie", "cuo|ci", "ji", "shi", "ruo", "song", "zong", \
	"jiang", "jiao|liao", "kang", "chan|yan", "die|di", "can|cen", "ding", "tu", "lou", "zhang", \
	"zhan|chan", "zhan|chan", "ao", "cao", "qu", "qiang", "wei|cui|zui", "zui", "dao", "dao", \
	"xi", "yu", "pei|pi", "long", "xiang", "ceng|zheng", "bo", "qin", "jiao", "yan", \
	"lao", "zhan", "lin", "liao", "liao", "jin|qin", "deng", "duo|tuo", "zun", "qiao|jiao", \
	"jue|gui", "yao", "jiao", "yao", "jue", "zhan|shan", "yi", "xue", "nao", "ye", \
	"ye", "yi", "nie", "xian|yan", "ji", "jie|xie", "ke", "gui|xi", "di", "ao", \
	"zui", "wei", "ni|yi", "rong", "dao", "ling", "jie", "yu", "yue", "yin", \
	"ru", "jie", "li|lie", "xi|gui", "long", "long", "dian", "ying|hong", "xi", "ju", \
	"chan", "ying", "kui|wei", "yan", "wei", "nao", "quan", "chao", "cuan", "luan", \
	"dian", "dian", "nie", "yan", "yan", "yan", "kui|nao", "yan", "chuan|shun", "kuai", \
	"chuan", "zhou", "huang", "jing|xing", "shun|xun|yan", "chao", "chao", "lie", "gong", "zuo", \
	"qiao", "ju|qu", "gong", "keo", "wu", "pu", "pu", "cha|chai|ci", "qiu", "qiu", \
	"ji|qi", "yi", "si|yi", "ba", "zhi", "zhao", "xiang|hang", "yi", "jin", "sun", \
	"quan", "phas", "xun", "jin", "fu|po", "za", "bi|yin", "fu|shi", "bu", "ding", \
	"shuai", "fan", "nie", "shi", "fen", "pa", "zhi", "xi", "hu", "dan", \
	"wei", "zhang", "nu|tang", "dai", "wa|mo", "pei|pi", "mo|pa", "tie", "bo|fu", "chen|lian", \
	"zhi", "zhou", "bo", "zhi", "di", "mo", "yi", "yi", "ping", "qia", \
	"juan", "ru", "shuai", "dai", "zhen|zheng", "shui", "qiao", "zhen", "shi", "qun", \
	"xi", "bang", "dai", "gui", "chou|dao", "ping", "zhang", "jian|san", "wan", "dai", \
	"wei", "chang", "qie|sha", "qi|ji", "ce|ze", "guo", "mao", "zhu|du", "hou", "zhen|zheng", \
	"zheng|xu", "mi", "wei", "wo", "bi|fu", "kai|yi", "bang", "ping", "die", "gong", \
	"pan", "huang", "tao", "mi", "jia", "teng", "hui", "zhong", "shen|shan|qiao", "man", \
	"man|mu", "biao", "guo", "ce|ze", "mu", "bang", "zhang", "jing", "chan", "fu", \
	"zhi", "wu|hu", "fan", "zhuang|chuang", "bi", "bi", "zhang", "mi", "qiao", "chan", \
	"fen", "meng", "bang", "chou|dao", "mie", "chu", "jie", "xian", "lan", "an|gan", \
	"beng|bing|pian|ping", "nian|ning", "jian", "bing", "bing", "nie|xing", "gan", "mi|yao", "huan", "yao|you", \
	"you", "ji", "guang|an", "bi|pi", "ting", "ze", "guang", "peng|zhuang", "me|mo", "qiang|qing", \
	"bi|pi", "qin|qi", "tun|dun", "chuang", "gui", "ya", "xin|ting|bai", "jie", "xu", "lu|lv", \
	"wu", "zhuang", "ku", "ying", "de|di", "pao", "dian", "ya", "miao", "geng", \
	"ci", "fu|zhou", "tong", "long|pang", "fei", "xiang", "yi", "zhi", "tiao", "zhi", \
	"xiu", "du|duo", "zuo", "xiao", "tu", "gui", "ku", "pang|mang|meng", "ting", "you", \
	"bu", "bing|ping", "cheng", "lai", "bei|bi", "cuo|ji", "an|e|yan", "shu|zhe|zhu", "kang", "yong", \
	"tuo", "song", "shu", "qing", "yu", "yu", "miao", "sou", "ce|ci", "xiang", \
	"fei", "jiu", "e", "gui|wei|hui", "liu", "xia|sha", "lian", "lang", "sou", "zhi", \
	"bu", "qing", "jiu", "jiu", "jin|qin", "ao", "kuo", "lou", "yin", "liao", \
	"xi|dai", "lu", "yi", "chu", "chan", "tu", "si", "xin|qian", "miao", "chang", \
	"wu", "fei|fa", "guang|an", "kos", "kuai", "bi", "qiang|se", "xie", "lan|lin", "lan|lin", \
	"liao", "lu|lv", "ji", "ying", "xian", "ting", "yong", "li", "ting", "pianpang|yin", \
	"shun|xun|yan", "yan", "ting", "di", "po", "jian", "hui", "nai", "hui", "gong|pianpang", \
	"nian", "kai", "bian|pan", "yi", "qi", "nong|long", "fen", "qu|ju", "nan|yan", "yi", \
	"zang", "bi", "yi", "yi", "er", "san", "shi|te", "er", "shi", "shi", \
	"gong", "di|diao", "yin", "hu", "fu", "hong", "wu", "di|ti", "chi", "jiang", \
	"ba", "shen", "di|ti|tui", "zhang", "jue|zhang", "tao", "fu", "di", "mi", "xian", \
	"hu", "chao", "nu", "jing", "zhen", "yi", "mi", "juan|quan", "wan", "shao", \
	"ruo", "yuan|xuan", "jing", "diao", "zhang", "jiang", "qiang|jiang", "peng", "dan|tan", "qiang|jiang", \
	"bi", "bi", "she", "dan|tan", "jian", "gou|kao", "ge", "fa", "bi", "kou", \
	"jian", "bie", "xiao", "tan|dan", "guo|kuo", "jiang|qiang", "hong", "mi|ni", "kuo|guo", "wan", \
	"jue", "xue|ji", "ji", "gui", "dang", "lu", "lu", "duan|shi|tuan", "hui", "zhi", \
	"hui", "hui", "yi", "yi", "yi", "yi", "huo|yue", "huo|yue", "xian", "xing", \
	"wen", "tong", "yan", "pan|yan", "yu", "chi", "cai", "biao", "diao", "ban|bin", \
	"bang|pang|peng", "yong", "miao|piao", "zhang", "ying", "chi", "chi|fu", "zhuo|bo", "yi|tuo", "ji", \
	"fang|pang", "zhong", "yi", "wang", "che", "bi", "di", "ling", "fu", "wang", \
	"zheng", "cu|zu", "wang", "jing", "dai", "xi", "xun", "hen", "yang", "huai|hui", \
	"lv", "hou", "jia|wa|wang", "cheng|zheng", "zhi", "xu", "jing", "tu", "cong", "cong|xi", \
	"lai", "cong", "de|dei", "pai", "si|xi", "uu", "ji", "chang", "zhi", "cong|zong", \
	"zhou", "lai", "wu|ya|yu", "xie", "jie", "jian", "shi|ti", "jia|xia", "bian|pian", "huang", \
	"fu", "xun", "wei", "pang|fang|bang", "yao", "wei", "xi", "zheng", "biao|piao", "chi|ti", \
	"de", "zheng|zhi", "zheng|zhi", "bie", "de", "chong|zhong", "che", "yao|jiao", "hui", "jiao", \
	"hui", "mei", "long", "rang|xiang", "bao", "qu|ju", "xin", "xin", "bi", "yi", \
	"le", "ren", "dao", "ding|ting", "gai", "ji", "ren", "ren", "chan|qian", "keng|tan", \
	"dao|te", "tui|tei|te", "gan|han", "qi|yi", "tai|shi", "cun", "zhi", "wang", "mang", "lie|xi", \
	"fan", "ying", "tian", "wen|min", "wen|min", "zhong", "chong", "wu", "ji", "wu", \
	"xi", "jie|jia", "you", "wan", "cong", "song|zhong", "kuai", "yu|shu", "bian", "qi|zhi", \
	"shi|qi", "cui", "chen|dan", "tai", "dun|tun|zhun", "qin|qian", "nian", "hun", "xiong", "niu", \
	"wang|kuang", "xian", "xin", "hang|kang", "hu", "kai|xi", "fen", "fu|huai", "tai", "song", \
	"wu", "ou", "chang", "chuang", "ju", "yi", "bao", "chao", "min|men", "pei", \
	"zuo|zha", "zen", "yang", "ju|kou", "ban", "nu", "nao|niu", "zheng", "bo|pa", "bu", \
	"zhan|tie", "gu|hu", "hu", "ju|qu", "da|dan", "lian|ling", "si|sai", "you|chou", "di", "dai|yi", \
	"yi", "die|tu", "you", "fu", "ji", "peng", "xing", "yuan|yun", "ni", "guai", \
	"bei|fei|fu", "xi", "bi", "you|yao", "qie", "xuan", "cong", "bing", "huang", "xu|xue", \
	"chu|xu", "pi|bi", "shu", "xi", "tan", "yong", "long|zong", "dui", "mo|mi", "ki", \
	"yi", "shi", "nen|nin", "shun|xun", "shi|zhi", "xi", "lao", "heng|geng", "kuang", "mou", \
	"zhi", "xie", "lian", "tiao|yao", "guang|huang", "die", "hao", "kong", "wei|gui", "heng", \
	"qi|xu", "jiao|xiao", "shu", "si", "kua|hu", "qiu", "yang", "hui", "hui", "chi", \
	"jia|qi", "yi", "xiong", "guai", "lin", "hui", "zi", "xu", "chi", "shang", \
	"nv", "hen", "en", "ke", "dong|tong", "tian", "gong", "quan|zhuan", "xi", "qia", \
	"yue", "peng", "ken", "de", "hui", "e|wu", "xiao", "tong", "yan", "kai", \
	"ce", "nao", "yun", "mang", "tong|yong", "yong", "juan|yuan", "bi|pi", "kun", "qiao", \
	"yue", "yu|shu", "yu|tu", "jie|ke", "xi", "zhe", "lin", "ti", "han", "hao|jiao", \
	"qie", "ti", "bu", "yi", "qian", "hui", "xi", "bei", "man|men", "yi", \
	"heng", "song", "quan|xun", "cheng", "kui|li", "wu", "wu", "you", "li", "liang|lang", \
	"huan", "cong", "nian|yi", "yue", "li", "nin", "nao", "e", "que", "xuan", \
	"qian", "wu", "min", "cong", "fei", "bei", "de", "cui", "chang", "men", \
	"li", "ji", "guan", "guan", "xing", "dao", "qi", "kong", "tian", "lun", \
	"xi", "kan", "gun", "ni", "qing", "chou|dao|qiu", "dun", "guo", "zhan", "jing|liang", \
	"wan", "yuan|wan", "jin", "ji", "lin|lan", "yu|xu", "huo", "he", "juan|quan", "tan|dan", \
	"ti", "ti", "nian|nie", "wang", "chuo|chui", "hu", "hun|men", "xi", "chang", "xin", \
	"wei", "hui", "e|wu", "rui|suo", "zong", "jian", "yong", "dian", "ju", "can", \
	"cheng", "de", "bei", "qie", "can", "dan|da", "guan", "duo|tuo", "nao", "yun", \
	"xiang", "chuan|gua|zhui", "die|tie", "huang", "chun", "qiong", "re|ruo", "xing", "ce", "bian", \
	"min|hun", "zong", "shi|ti", "qiao|qiu", "chou|jiu|qiao", "bei", "xuan", "wei", "ge", "qian", \
	"wei", "yu", "tou|yu", "bi", "xuan", "huan", "fen|min", "bi", "yi", "mian", \
	"yong", "he|qi|kai", "yang|tang|shang|dang", "yin", "e", "chen|xin|dan", "mao", "ke|qia", "ke", "yu", \
	"ai", "qie", "yan", "nuo|ruan", "gan|han", "wen|yun", "cong|song", "si|sai", "leng", "fen", \
	"ying", "kui", "kui", "que", "gong|hong", "yun", "su", "su|shuo", "qi", "yao", \
	"song", "huang", "ji", "gu", "ju", "chuang", "ni", "xie", "kai", "zheng", \
	"yong", "cao", "xun", "shen", "bo", "kai|xi", "yuan", "xi|xie", "hun", "yong", \
	"yang", "li", "sao|cao", "tao", "yin", "ci", "chu|xu", "qian|qie", "tai", "huang", \
	"wen|yun", "shen|zhen", "ming", "gong", "she", "cao|cong", "piao", "mu", "mu", "guo", \
	"chi", "can", "can", "can", "cui", "min", "ni|te", "zhang", "tong", "ao", \
	"shuang", "man", "guan", "que", "zao", "jiu", "hui", "kai", "lian", "ou", \
	"song", "jin|qin", "yin", "lv", "shang", "wei", "tuan", "man", "qian|xian", "she", \
	"yong", "qiang|qing", "kang", "chi|di", "zhi|zhe", "lou|lv", "juan", "qi", "qi", "yu", \
	"ping", "liao", "cong|song", "you", "chong", "zhi", "tong", "cheng", "qi", "qu", \
	"peng", "bei", "bie", "qiong", "jiao", "zeng", "chi", "lian", "ping", "kui", \
	"hui", "qiao", "cheng|zheng", "yin|xin", "yin", "xi", "xi", "dan|da", "tan", "duo", \
	"dui", "dun|tun|dui", "su", "jue", "ce", "jiao|xiao", "fan", "fen", "lao", "lao", \
	"chong|zhuang", "han", "qi", "xian", "min", "jing", "liao", "wu", "can", "jue", \
	"cu", "xian", "tan", "sheng", "pi", "yi", "chu", "xian", "nao|nang", "dan", \
	"tan", "jing", "song", "dan|han", "ji|jiao", "wei", "huan|xuan", "dong", "qin", "qin", \
	"ju", "cao|sao", "ken", "xie", "ying", "ao|yu", "mao", "yi", "lin", "se", \
	"jun", "huai", "men", "lan", "ai", "lan|lin", "yan|ye", "guo|kuo", "xia", "chi", \
	"yu", "yin", "dai", "meng", "yi|ni|ai", "meng", "dui", "qi|ji", "mo", "lan|xian", \
	"men", "chou", "zhi", "nuo", "nuo", "yan|chu", "yang", "bo", "zhi", "kuang", \
	"kuang", "you", "fu", "liu", "mie", "cheng", "hui|sui", "chan", "meng", "lai|lan", \
	"huai", "xuan", "rang", "chan", "ji", "ju", "huan|guan", "she", "yi", "lian", \
	"nan", "mi|mo", "tang", "jue", "gang|zhuang", "gang|zhuang", "gang|zhuang", "ge", "yue", "wu", \
	"jian", "xu|qu", "shu", "reng|rong", "xi|hu", "cheng", "wo", "jie", "ge", "can|jian", \
	"qiang|zang", "huo|yu", "qiang", "zhan", "dong", "cu|qi", "jia", "die", "cai", "jia", \
	"ji", "zhi", "kan|zhen", "ji", "kui", "gai", "deng", "zhan", "chuang|qiang", "ge", \
	"jian", "jie", "yu", "jian", "yan|you", "lu", "xi|hu", "zhan", "xi|hu", "xi|hu", \
	"chuo", "dai", "qu", "hu", "hu", "hu", "e", "shi|yi", "ti", "mao", \
	"hu", "li", "fang|pang", "suo", "bian|pian", "dian", "jiong", "jiong|shang", "yi", "yi", \
	"shan", "hu", "fei", "yan", "shou", "ti", "cai|zai", "zha|za", "qiu", "le|li", \
	"pi|pu", "ba|pa", "da", "reng", "fan|fu", "ru", "zai", "tuo", "zhang", "diao|di", \
	"kang|gang", "yu|wu", "wu|ku", "gan|han", "shen", "cha|chai", "tuo|chi|yi", "ge|gu", "kou", "wu", \
	"den", "qian", "zhi", "ren", "guang|kuo|tang", "men", "sao", "yang", "chou|niu|zhou", "ban|fen|huo", \
	"che", "rao|you", "xi|cha", "qin|qian", "ban|pan", "jia", "yu", "fu|pu", "ao|ba", "zhe|xi", \
	"pi", "qi|zhi", "kan|sun|zhi", "e", "den", "hua|zhao", "cheng|zhang|zheng", "ji|qi", "yan", "wang|kuang", \
	"bian", "chao|suo", "gou|ju", "wen", "gu|hu", "yue", "jue", "ba|pa", "qin", "shen|dan", \
	"zheng", "yun", "wan", "ne|ni", "yi", "shu", "zhua", "pou", "dou|tou", "dou", \
	"gang|kang", "zhe|she", "fu|pou", "fu|mo", "pao", "ba", "niu|ao", "ze", "tuan|zhuan", "kou", \
	"liu|lun", "qiang|cheng", "yun|jun", "hu", "bao", "bing", "zhi|zhai", "beng|peng", "nan", "ba|bu|pu", \
	"pi", "chi|tai", "tao|yao", "zhen", "zha", "yang", "bao|pao|pou", "he|qia", "ni", "she|ye", \
	"di|qi|zhi", "chi", "pei|pi", "jia", "mo|ma", "mei", "chen|shen", "jia|xia|ya", "chou", "qu", \
	"min", "zhu|chu", "jia|ya", "bi|fei|fu|pi", "zha|zhan", "zhu", "dan|jie", "chai|ca", "mu", "dian|nian", \
	"la", "bu|fu", "pao", "ban|pan", "bo|pai", "lin|ling", "na", "guai", "qian", "ju", \
	"tuo|ta", "ba|bie|bo|fa", "chi|tuo", "chi|tuo", "niu|ao", "ju|gou", "zhuo", "pin|pan|fan", "qiao|shao|zhao", "bai", \
	"bai", "di|zhi", "ni", "ju", "kuo", "long", "jian", "qia", "yong", "lan", \
	"ning", "bo|fa", "ze|zhai", "qian", "hen", "kuo|gua", "shi", "jie|jia", "zheng", "nin", \
	"gong|ju", "gong", "quan", "quan|shuan", "cun|zun", "zan|za", "kao", "yi|chi|hai", "xie", "ce|chuo|se", \
	"hui", "bing|pin", "zhuai|ye", "shi|she", "na", "bai", "chi", "gua", "die|zhi", "guang|kuo", \
	"duo", "duo", "zhi", "jia|qia|qie", "an", "nong", "zhen", "ge|he", "jiao", "kua|ku", \
	"dong", "ru|na", "diao|tao|tiao", "lie", "zha", "lu", "she|die", "wa", "jue", "lie", \
	"ju", "zhi", "lian|luan", "ya", "wo|zhua", "ta", "jia|xie", "nao|rao|xiao", "dang", "jiao|kao", \
	"zheng", "ji", "hui|hun", "xian", "yu", "ai", "tuo|shui", "nuo", "cuo|zuo", "bo", \
	"geng", "ti", "zhen", "cheng", "suo|sha", "suo|sa|sha", "keng|qian", "mei", "nong", "ju", \
	"bang|beng|peng", "jian", "yi", "ting", "shan|yan", "nuo|sui|ruo|rua", "wan", "jia|xie", "cha|zha", "feng|peng", \
	"ku|jiao", "wu", "jun", "qiu|jiu|ju", "tong", "hun|kun", "chi|huo", "tu|shu|cha", "zhuo", "fu|pou", \
	"lv|luo", "ba|bie", "gan|han|xian", "qiao|shao|xiao", "nie", "juan|yuan", "ze", "song|shu|sou", "ye|yu", "jue|zhuo", \
	"bu", "wan", "bu|pu|zhi", "zun", "zhuai|ye", "zhai", "lu", "sou", "shui|tuo", "lao", \
	"sun", "bang", "jian", "huan", "dao", "wei", "wan|yu", "qin", "feng|peng", "she", \
	"li|lie", "min", "men", "fu|bu", "ba|bai|bi", "ju", "dao", "luo|wo", "ai", "juan|quan", \
	"yue", "song|zong", "tian|chen", "chui|duo", "cha|jie|qie", "tu", "ben", "na", "nian|nie", "wei|wo|re|ruo", \
	"cu|zuo", "wo|xia", "qi", "hen|xian", "cheng", "dian", "sao", "lun", "qing|qian", "gang", \
	"duo|zhuo", "shou", "diao|nuo", "fu|pei|pou", "di", "zhang", "hun", "ji|yi", "tao", "qia", \
	"qi", "bai|pai", "shu", "qian|wan", "ling", "ye", "ya", "jue|ku", "zheng", "liang", \
	"gua", "ni|yi|nie|nai", "huo|xu", "yan|shan", "ding|zheng", "lue", "cai", "tan|xian", "che", "bing", \
	"cha|jie|sha|xie", "ti", "kong|qiang", "tui", "yan", "ci|cuo|ze", "chou|zhou|zou", "ju", "tian", "qian", \
	"ken", "bai|bo", "pa|shou", "jie", "lu", "guo|guai", "ming", "geng|jie", "zhi", "dan|shan", \
	"meng", "chan|can|shan", "sao", "guan", "peng", "chuan|yuan", "nuo", "jian", "zheng|keng", "you|jiu", \
	"jian|qian", "chou|shu|you|yu", "yan", "kui", "nan", "hong|xuan", "rou", "pi|che", "wei", "sai|cai", \
	"cou|zou", "xuan", "mao|miao", "ti|di", "nie", "cha|zha", "shi", "song|zong", "zhen", "ji|yi", \
	"xun", "huang|yong", "bian", "yang", "huan", "yan", "zan|zuan", "an|yan|ye", "xu|ju", "ya", \
	"ou|wo", "ke|qia", "chuai|zhui", "ji", "di|ti", "la", "la", "cheng|chen", "jia|kai", "jiu", \
	"jiu", "tu", "jie|qi", "hui|hun", "gen", "chong|dong", "xian|xiao", "die|ye|she", "jia|xie", "huan|yuan", \
	"jian|qian", "ye", "cha|zha", "zha", "bei", "yao", "wei", "dem", "lan", "wen|wu", \
	"qin", "chan|shan", "ge", "lou", "zong", "gen", "jiao", "gou", "qin", "rong", \
	"huo|que", "zou|chou", "chi|chuai|yi", "zhan", "sun", "sun", "bo", "chu", "rong|nang", "bang|peng", \
	"chai|cuo|guo", "sao", "ke|e", "yao", "dao", "zhi", "nuo|nu|nou", "xie|la", "jian|lian", "shao|sou|xiao", \
	"qiu", "gao|kao|qiao", "xian", "shuo", "sang", "jin", "mie", "yi|e", "dui|chui", "nuo", \
	"shan", "da|ta", "jie|zhe|zha", "tang", "ban|pan|po", "ban|su", "da|ta", "li", "tao", "hu|ku", \
	"zhi|nai", "wa", "xia|hua", "qian", "wen", "qiang|cheng", "shen|tian", "zhen", "e", "xie", \
	"na|nuo", "quan", "cha", "zha", "ge", "wu", "en", "nie|sha|she|zhe", "gang|kang", "she|nie", \
	"lu|shu", "bai", "yao", "bin", "rong", "nan|tan", "sha|sa|shai", "chan|sun", "suo", "liao|nao|jiao|jiu|liu", \
	"chong", "chuang", "guo|guai", "bing", "feng|peng", "shuai", "tu|zhi|di", "ji|qi|cha", "song|sou", "zhai", \
	"lian", "cheng", "chi", "guan", "lu", "luo", "lou", "zong", "gai|xi", "hu|chu", \
	"zhua|zha", "cheng|qiang", "tang", "hua", "cui|cuo|zui", "nai|zhi", "mo|ma", "jiang|qiang", "gui", "ying", \
	"zhi", "ao|qiao", "zhi", "che|nie", "man", "can|chan", "kou", "chu|chi", "she|se|su|mi", "tuan|zhuan", \
	"chao|jiao", "mo", "mo", "zhe|la", "chan|can|shan|xian", "qian|keng", "biao|pao|piao", "jiang", "yao", "gou", \
	"qian", "liao", "ji", "ying", "jue", "bie|pie", "bie|pie", "lao", "dun", "xian", \
	"ruan|ruo", "gui", "zan|qian|zen", "yi", "xun|xian", "cheng", "cheng", "sa", "nao|xiao", "hong", \
	"si|xi", "han|qian", "heng|guang", "da|ta", "zun", "nian", "lin", "cheng|zheng", "wei|hui", "zhuang", \
	"jiao|kao", "ji", "cao", "tan|dan", "dan|shan", "che", "bo|fa", "che", "jue", "xiao|sou", \
	"lao|liao", "ben", "fu|mo", "qiao", "bo", "cuo|zuo", "zhuo", "suan|xuan|zhuan", "zhui|tuo|wei", "pu|bu", \
	"qin", "dun", "nian", "hua", "xie", "lu", "jiao", "cuan", "ta", "han", \
	"ji|yao|qiao", "wo|zhua", "jian|lian", "gan", "yong", "lei", "nang", "lu", "shan", "zhuo", \
	"ze|zhai", "bu|pu", "chuo", "ji|xi", "dang", "se", "cao", "qing", "jing|qing", "huan|juan|xuan", \
	"jie", "qin", "kuai", "dan|shan", "xie", "jia|qia|ye", "pi|bo", "bo|bai", "ao", "ju", \
	"ye", "e", "meng", "sou", "mi", "ji", "tai", "zhuo", "chou|dao", "xing", \
	"lan", "ca", "ju", "ye", "nou|nu|ru", "ye", "ye", "ni", "hu|huo", "jie", \
	"bin", "ning", "ge", "zhi", "jie|zhi", "guang|kuo|tang", "mi|mo", "jian", "xie", "la|lie", \
	"tan", "bai", "sou", "lu", "lue|li", "rao", "zhi|ti", "pan", "yang", "lei", \
	"ca|sa", "lu|shu", "cuan", "nian", "xian", "jun|pei", "huo|que", "li", "lai|la", "huan", \
	"ying", "lu|luo", "long", "qian", "qian", "zan|cuan", "qian", "lan", "xian|jian", "ying", \
	"mei", "ning|rang|xiang", "chan|shan", "ying", "cuan", "xie", "nie|sha|she|zhe", "luo", "mei", "mi|mo", \
	"chi", "zan|cuan", "luan|lian", "tan|nan", "zuan", "li|shai", "dian", "wa", "dang|tang", "jiao", \
	"jue", "lan", "li|luo", "nang", "qi|zhi", "gui", "gui", "ji|qi|yi", "xun", "po|pu", \
	"fan|pu", "shou", "kao", "you", "gai", "yi", "gong", "gan|han", "ban", "fang", \
	"zheng", "po", "dian", "kou", "fen|min", "wu|mou", "gu", "he|shi", "ce", "xiao", \
	"mi", "shou|chu", "ge|guo|e", "di|hua", "xu", "jiao", "min", "chen", "jiu", "zhen", \
	"dui|duo", "yu", "chi|sou", "ao", "bai", "xu", "jiao", "dui|duo", "lian", "nie", \
	"bi", "chang|cheng|zheng", "dian", "duo|que", "yi", "gan", "san", "ke", "yan|jiao", "dun|dui", \
	"qi|ji|yi", "tou", "xue|xiao", "duo|que", "jiao|qiao", "jing", "yang", "xia|gui", "min", "shu|shuo", \
	"ai|zhu", "qiao", "ai|zhu", "zheng", "di", "chen|zhen", "fu", "shu|shuo", "liao", "ou|qu", \
	"xiong|xuan", "yi", "jiao", "shan", "jiao", "zhu|zhuo", "yi|du", "lian", "bi", "li|tai", \
	"xiao", "xiao", "wen", "xue", "qi", "qi", "zhai", "bin", "jue|jiao", "zhai", \
	"uu", "fei", "ban", "ban", "lan", "yu|zhong", "lan", "men|wei", "dou|zhu", "sheng", \
	"liao", "jia", "hu", "cha|xie|ye", "jia", "yu", "zhen", "jiao", "wo|guan", "tou|tiao", \
	"dou", "jin", "che|chi|zhe", "yin|zhi", "fu", "qiang", "zhan", "qu", "chuo|zhuo", "zhan", \
	"duan", "zhuo", "shi|si", "xin", "zhuo", "zhuo", "jin|qin", "lin", "zhuo", "chu", \
	"duan", "zhu", "fang|pang", "jie|chan", "hang", "yu|wu", "shi|yi", "pei", "you|liu", "myeo", \
	"bang|beng|pang|peng", "qi", "zhan", "mao|wu", "lv", "pei", "pi|bi", "liu", "fu", "fang", \
	"xuan", "jing", "jing", "ni", "cou|sou|zou|zu", "zhao", "yi", "liu", "shao", "jian", \
	"eos", "yi", "qi", "zhi", "fan", "piao", "fan", "zhan", "kuai", "sui", \
	"yu", "wu|mo", "ji", "ji|xi", "ji", "huo", "ri", "dan", "jiu", "zhi", \
	"zao", "xie", "tiao", "jun|xun", "xu", "ga|xu", "la", "gan|han", "han", "tai|ying", \
	"di", "xu", "chan", "shi", "kuang", "yang", "shi", "wang", "min", "min", \
	"tun|zhen", "chun", "wu", "yun", "bei", "ang|yang", "ze", "ban", "jie", "hun|kun", \
	"sheng", "hu", "fang", "hao", "jiong|gui", "chang", "xuan", "meng|ming", "hun", "fen", \
	"qin", "hu", "yi", "cuo|xi", "cuan|xin", "yan", "ze", "fang", "tan|yu", "shen", \
	"ju", "yang", "zan", "bing|fang", "xing", "yang|ying", "xuan", "po|pei", "zhen", "ling", \
	"chun", "hao", "mei|mo|wen", "zuo", "mo", "bian", "xiong|xu", "hun", "zhao", "zong", \
	"shi|ti", "shi|xia", "yu", "fei", "yi|die", "mao", "ni|zhi", "chang", "on|wen", "dong", \
	"ai", "bing", "ang", "zhou", "long", "xian", "kuang", "tiao", "chao|zhao", "shi", \
	"huang", "huang", "xuan", "kui", "kua|xu", "jiao", "jin", "zhi", "jin", "shang", \
	"tong", "hong", "yan", "gai", "xiang", "shai", "xiao", "ye", "yun", "hui", \
	"han", "han", "jun", "wan", "xian", "kun", "zhou", "xi", "cheng|sheng", "sheng", \
	"bu", "zhe", "zhe", "wu", "wan|han", "hui", "hao", "chen", "wan", "tian", \
	"zhuo", "zui", "zhou", "pu", "jing|ying", "xi", "shan", "ni", "xi", "qing", \
	"du|qi", "jing", "gui", "zheng", "yi", "zhi", "yan|an", "wan", "lin", "liang", \
	"chang|cheng", "wang", "xiao", "zan", "fei", "xuan", "geng|xuan", "yi", "jia|xia", "yun", \
	"hui", "xu", "min", "kui", "ye", "ying", "du|shu", "wei", "shu", "qing", \
	"mao", "nan", "jian|lan", "nuan|xuan", "an", "yang", "chun", "yao", "suo", "pu", \
	"ming", "jiao", "kai", "gao|hao", "weng", "chang", "qi", "hao", "yan", "li", \
	"ai", "ji|jie", "ji", "men", "zan", "xie", "hao", "mu", "mo|mu", "cong", \
	"ni", "zhang", "hui", "bao|bo|pu", "han", "xuan", "chuan", "liao", "xian", "tan", \
	"jing", "pie", "lin", "tun", "xi", "yi", "ji|jie", "huang", "dai", "ye", \
	"ye", "li", "tan", "tong", "xiao", "fei", "shen", "zhao", "hao", "yi", \
	"shan|xiang", "xing", "shan", "jiao", "bao", "jing", "yan", "ai", "ye", "ru", \
	"shu", "meng", "xun", "yao", "bao|pu", "li", "chen", "kuang", "die", "uu", \
	"yan|yao", "huo", "lu|lv", "xi", "rong", "long", "nang", "luo", "luan", "shai", \
	"tang", "yan", "zhu", "yue", "yue|zad", "qu", "ye", "geng", "yi", "hu", \
	"e|he", "shu", "cao", "cao", "sheng", "man", "zeng|ceng", "zeng|ceng", "ti", "zui|cuo", \
	"jian|can|qian", "xu", "hui|kuai", "yin", "qie", "fen", "pi", "ru|yue", "wei|you", "ruan|wan", \
	"peng", "fen|ban", "bi|bo|fu", "ling", "ku|fei", "xu|qu|chun", "uu", "nv", "tiao", "shuo", \
	"zhen", "lang", "lang", "juan|zui", "ming", "mang|huang", "wang", "tun", "chao|zhao", "qi", \
	"qi|ji", "ying", "zong", "wang", "tong|chuang", "lang", "lao", "mang|meng", "long", "mu", \
	"pin|teun", "wei", "mo", "ben|pen", "ya|zha", "shu|zhu", "shu|zhu", "teul", "shu|zhu", "ren", \
	"ba", "piao|pu|po", "duo", "duo", "dao|mu", "li", "qiu|gui", "ji|wei", "jiu", "bi", \
	"xiu", "zhen|cheng", "ci", "sha", "ru", "duo|za", "quan", "qian", "yu|wu", "gan", \
	"wu", "cha", "shan|sha", "xun", "fan", "wo|wu", "zi", "li", "xing", "cai", \
	"cun", "er|ren", "shao|biao", "tuo|zhe", "duo|di", "zhang", "mang", "chi", "yi", "ge|gu|gai", \
	"gong", "du|tu", "li|yi", "qi", "shu", "gang|gong", "tiao", "jiang", "shan", "wan", \
	"lai", "jiu", "mang", "yang", "ma", "miao", "zhi|si|xi", "yuan", "hang|kang", "fei|bei", \
	"bei", "jie", "dong", "gao", "yao", "qian|xian", "chu", "chun", "ba|pa", "shu|dui", \
	"hua", "xin", "niu|chou", "shu|zhu", "chou", "song", "ban", "song", "ji", "yue|wo", \
	"jin", "gou", "ji", "mao", "bi|pi", "bi|pi|mi", "kuang|wang", "ang", "fang|bing", "fen", \
	"yi", "fu", "nan", "si|xi", "hu|di", "ya|ye", "dou|zhu", "xin", "chen|zhen", "yao", \
	"lin", "nen|rui", "e", "mei", "zhao", "guan|guo|luo", "zhi|qi", "cong|zong", "yun", "zui", \
	"sheng", "shu", "zao", "duo|di", "li", "lu", "jian", "cheng", "song", "qiang", \
	"feng", "zhan|nan", "xiao", "zhen|xian", "gu|ku", "ping", "tai|si|ci", "xi", "zhi", "guai", \
	"xiao", "jia", "jia", "gou|ju", "bao|fu", "mo", "xie|yi", "ye", "ye", "shi", \
	"nie", "bi", "tuo|duo", "yi|li", "ling", "bing", "ni|chi", "la", "he", "ban|pan", \
	"fan", "zhong", "dai", "ci", "yang|ying", "fu", "bai|bo", "mei|mou", "gan|qian", "qi", \
	"ran", "rou", "mao|shu", "shao", "song", "zhe", "jian|xia", "you|zhou", "shen", "gui|ju", \
	"tuo", "zha|zuo", "ran|nan", "chu|ning|zhu", "yong", "chi|di", "zhi|die", "zu|ju|zha", "zha|cha", "dan", \
	"gu", "bu|pu", "jiu", "ao", "fu", "jian", "ba|bo|bie|fu|pei", "duo|zuo|wu", "ke", "nai", \
	"zhu", "bi|bie", "liu", "chai|ci|zhai", "zha|shan", "si", "zhu", "pei|bei", "shi|fei", "guai", \
	"zha|cha", "yao", "cheng|jue", "jiu", "shi", "zhi", "liu", "mei", "li", "rong", \
	"zha|shan|ce|shi", "zao", "biao", "zhan", "zhi", "long", "dong", "lu", "saeng", "li|yue", \
	"lan", "yong", "shu|sun", "xun|sun", "shuan", "qi|qie", "chen|zhen", "qi|xi", "li|lie", "yi", \
	"xiang", "zhen", "li", "ci|se", "kuo|gua|tian", "kan", "bing|ben", "ren", "xiao|jiao", "bai", \
	"ren", "bing", "zi", "chou", "xie|yi", "ci", "xu|yu", "zhu", "zun|jian", "zui", \
	"er", "er", "you|yu", "fa", "gong", "kao", "lao", "zhan", "lie|li", "yin", \
	"yang", "he|hu", "gen", "zhi|yi", "shi", "ge|he|luo", "zai|zhi", "luan", "fu", "jie", \
	"heng|hang", "gui", "tao|tiao|zhao", "guan|guang", "gui|wei", "kuang", "ru", "an", "an", "juan|quan", \
	"yi|ti", "zhuo", "ku", "zhi", "qiong", "dong|tong", "sang", "sang", "huan", "ju|jie", \
	"jiu", "xue", "duo", "chui", "mou|yu", "zan|za", "uu", "ying", "jie", "liu", \
	"zhan", "ya", "rao|nao", "zhen", "dang", "qi", "qiao", "hua", "hui|gui", "jiang", \
	"zhuang", "xun", "suo", "sa", "chen|zhen", "bei", "ying|ting", "kuo", "jing", "bo|po", \
	"ben|fan", "fu", "rui", "tong", "jue", "xi", "lang", "liu", "feng", "qi", \
	"wen", "jun", "gan|han", "su|yin", "liang", "qiu", "ting", "you", "mei", "bang", \
	"long", "peng", "zhuang", "di", "juan|xuan|xue", "tu|cha", "zao", "ao|you", "gu", "bi", \
	"di", "han", "zi", "zhi", "ren", "bei", "geng", "jian|xian", "huan", "wan", \
	"nuo", "jia", "tiao", "ji", "xiao", "lv", "hun|kuan", "shao|sao", "cen", "fen", \
	"song", "meng", "wu|yu", "li", "li|si|qi", "dou", "qin", "ying", "suo|xun", "ju", \
	"ti", "xie", "kun|hun", "zhuo", "shu", "chan", "fan", "wei", "jing", "li", \
	"bing|bin", "xia", "fo", "tao|chou|dao", "zhi", "lai", "lian", "jian", "tuo|zhuo", "ling", \
	"li", "qi", "bing", "lun", "song|cong", "qian", "mian", "qi", "ji|qi", "cai", \
	"ao|gun|hun", "chan", "de|zhe", "fei", "pai|pei|bei", "bang", "bei|pou|bang", "hun", "zong", "chang|cheng", \
	"zao", "ji", "li|lie", "peng", "yu", "yu", "gu", "gun|jun", "dong", "tang", \
	"gang", "wang", "di|ti|dai", "cuo|que", "fan", "cheng", "chen|zhan", "qi", "yuan", "yan", \
	"yu", "juan|quan", "yi", "sen", "shen|ren", "chui|duo", "leng|ling", "qi|xi", "zhuo", "fu|su", \
	"ke|kuan", "lai", "zou|sou", "zou", "zhuo|zhao", "guan", "fen", "fen", "chen", "qing", \
	"ni|nie", "wan", "guo", "lu", "hao", "jie|qie", "yi", "chou|zhou|diao", "ju", "ju", \
	"cheng|sheng", "zu|cui", "liang", "kong|qiang", "zhi", "zhui|chui", "ya|e", "ju", "bei|pi", "jiao", \
	"zhuo", "zi", "bin", "peng", "ding", "chu", "chang", "men", "hua", "jian", \
	"gui", "xi", "du", "qian", "dao", "gui", "dian", "luo", "zhi", "juan|quan", \
	"myeong", "fu", "geng", "peng", "shan", "yi", "tuo", "san|sen", "chuan|duo", "ye", \
	"fu", "wei|hui", "wei", "duan", "jia", "zong", "jian|han", "yi", "shen|zhen", "po|xi", \
	"yan|ya", "yan", "chuan", "jian|zhan", "chun", "yu", "he", "zha|cha", "wo", "pian", \
	"bi", "yao", "guo|huo|kua", "xu", "ruo", "yang", "la", "yan", "ben", "hui", \
	"kui", "jie", "kui", "si", "feng|fan", "xie", "tuo", "ji|zhi", "jian", "mu", \
	"mao", "chu", "ku|hu", "hu", "lian", "leng", "ting", "nan", "yu", "you", \
	"mei", "cong|song", "yuan|xuan", "xuan", "yang", "zhen", "pian", "ye|die", "ji", "jie|qia", \
	"ye", "chu|zhu", "dun|shun", "yu", "zou|cou", "wei", "mei", "di|ti", "ji", "jie", \
	"kai|jie", "qiu", "ying", "rou", "huang", "lou", "le|yue", "quan", "xiang", "pin", \
	"shi", "gai|gui|jie", "tan", "lan", "wen|yun", "yu", "chen", "lv", "ju", "shen", \
	"chu", "pi", "xie", "jia", "yi", "zhan|chan|zhen", "bo|fu", "nuo", "mi", "lang", \
	"rong", "gu", "jin|jian", "ju", "ta", "yao", "zhen", "bang|beng", "sha|xie", "yuan", \
	"zi", "ming", "su", "jia", "yao", "jie", "huang", "gan|han", "fei", "zha", \
	"qian", "ma", "sun", "yuan", "xie", "rong", "shi", "zhi", "cui", "wen", \
	"ting", "liu", "rong", "tang", "que", "zhai", "si", "sheng", "ta", "ke", \
	"xi", "gu", "qi", "gao|kao", "gao|kao", "sun", "pan", "tao", "ge", "chun", \
	"zhen|dian", "nou", "ji", "shuo", "gou|jie", "chui|zhui", "qiang|cheng", "cha", "qian|xian|lian", "huai", \
	"mei", "chu|xu", "gang", "gao", "zhuo", "tuo", "qiao", "yang", "zhen|dian", "jia", \
	"kan|jian", "zhi", "dao", "long", "bin|bing", "zhu", "sang", "xi|die", "ji|gui", "lian", \
	"hui", "yong|rong", "qian", "guo", "gai", "gai", "tuan|quan|shuan", "hua", "qi|se", "shen|sen", \
	"cui|zui|zhi", "peng", "you|chao", "hu", "jiang", "hu", "huan", "gui", "nie|xie|yi", "yi", \
	"gao", "kang", "gui", "gui", "cao|zao", "man|wan", "jin|qin", "di|zhe|zhi", "chong|zhuang", "le|yue", \
	"lang", "chen", "cong|zong", "li|chi", "xiu", "qing", "shuang|shang", "fan", "tong", "guan", \
	"ze", "su", "lei", "lu", "liang", "mi", "lou|lv", "chao|jiao", "su", "ke", \
	"chu", "cheng|tang", "biao", "lu|du", "liao|jiu", "zhe", "zha", "du|shu", "zhang", "lang|man", \
	"mo|mu", "niao|mu", "yang", "tiao", "peng", "zhu", "sha|xie", "xi", "quan", "guang|heng", \
	"jian", "cong", "ji", "yan", "qiang", "xue", "ying", "er|zhi", "xun", "zhi|yi", \
	"qiao", "zui", "cong", "pu", "shu", "hua", "gui|kui", "zhen", "zun", "yue", \
	"shan", "xi", "chun", "dian", "fa|fei", "gan", "mo", "wu", "qiao", "rao|nao", \
	"lin", "liu", "jiao|qiao", "jian|xian", "run", "fan", "zhan|jian", "du|luo|tuo", "liao|lao", "yun", \
	"shun", "dun|tui", "cheng", "tang|cheng", "meng", "ju", "chen|cheng|deng", "su|xiao|qiu", "jue", "jue", \
	"tan|dian", "hui", "ji", "nuo", "xiang", "duo|tuo", "ning", "rui", "zhu", "tong|chuang", \
	"zeng|ceng", "fei|fen", "qiong", "ran|yan", "heng", "qian|qin", "gu", "liu", "lao", "gao", \
	"chu", "xi", "sheng", "ca|zi", "san|zan", "ji", "dou", "jing", "lu", "jian", \
	"chu", "yuan", "da", "qiao|shu", "jiang", "shan|tan", "lin", "nao", "yin", "xi", \
	"hui", "shan", "zui", "xuan", "cheng", "gan", "ju", "zui", "yi", "qin", \
	"pu", "dan|yan", "lei", "feng", "hui", "dang", "ji", "sui", "bo|nie", "bo|ping", \
	"cheng", "chu", "zhua", "hui|gui", "ji", "jie", "jia", "jing|qing", "zhai|shi", "jian", \
	"qiang", "dao", "yi", "biao", "song", "she", "lin", "li", "cha|sa", "meng", \
	"yin", "tao|chou|dao", "tai", "mian", "qi", "tuan", "bin|bing", "huo|hua", "ji", "lian|qian", \
	"mi|ni", "ning", "yi", "gao", "jian|kan", "yin", "ru|ruan|nou", "qing", "yan", "qi", \
	"mi", "di|zhao", "gui", "chun", "ji", "kui", "po", "deng", "chu", "ge", \
	"mian", "you", "zhi", "guang|huang|gu|guo", "qian", "lei", "lei|lie", "sa", "lu", "li", \
	"cuan", "lv|chu", "mie|mei", "hui", "ou", "lv", "zhi", "gao", "du", "yuan", \
	"yue|li", "fei", "zhu|zhuo", "sou", "lian", "jiang", "chu", "qing", "zhu", "lu|lv", \
	"yan", "li", "zhu", "qin|chen", "ji|jie|jue", "e", "su", "huai|gui", "nie", "yu", \
	"long", "lai|la", "jiao|qiao", "xian", "gui|kwi", "ju", "xiao|qiu", "ling", "ying", "jian|shan", \
	"yin", "you", "ying", "rang|xiang", "nong", "bo", "chan|zhan", "lan|lian", "ju", "shuang", \
	"she", "zui|wei", "cong", "guan|quan", "qu", "cang", "jou", "yu", "luo", "li", \
	"cuan|zuan", "luan", "dang|tang", "jue|qu", "eom|yan", "lan", "lan", "zhu", "lei|luo", "li", \
	"ba", "nang", "yu", "ling", "guang", "qian", "ci|zi", "huan", "xin", "yu", \
	"huan|yu|yi", "qian|xian", "ou", "xu", "chao", "chu|qu|xi", "qi", "ke|ai|kai", "yin|yi", "jue", \
	"kai|xi", "xu", "he|xia", "yu", "kui", "lang", "kuan|xin", "sou|shuo", "xi", "ai|ei", \
	"qi|yi", "qi", "xu|chua", "chuai|chi", "qin|yin", "kuan|xin", "dan|kan|qian", "kuan|xin", "kan|ke", "chuan", \
	"sha|xia", "gua", "yin", "xin", "xie|ya", "yu", "qian", "xiao", "ye", "ge", \
	"wu|yang", "tan", "jin|qun", "ou", "hu", "ti|xiao", "huan", "xu", "fen|pen", "xi|yi", \
	"chi|xiao", "xu|chua", "xi|she", "uu", "lian|han", "chu", "yi", "e", "yu", "chuo", \
	"huan", "zheng|zhi", "zheng", "ci", "bu", "wu", "qi", "bu", "bu", "wai", \
	"ju", "qian", "zhi|chi", "se", "chi", "se", "zhong", "sui|suo", "sui", "li", \
	"ze|ji", "yu", "li", "gui|kui", "dai|e", "e", "si", "jian", "zhe", "wen|mo", \
	"mo", "yao", "mo|wen", "cu", "yang", "tian", "sheng", "dai", "shang", "xu", \
	"xun", "shu", "can", "jing|jue", "bi|piao", "qia", "qiu", "su", "jing|qing", "yun", \
	"lian", "yi", "bo|fou|tou|ye", "zhi|shi", "yan|ye", "can", "hun|men|mei", "dan", "ji", "die", \
	"zhen", "yun", "wen", "chou", "bin", "ti", "jin", "shang", "yin", "diao", \
	"jiu", "kui|hui", "cuan", "yi", "dan", "du", "jiang", "lian", "bin", "du", \
	"jian", "jian", "shu", "ou", "duan", "zhu", "yin|yan", "sheng|qing|keng", "yi", "sa|shai|xie", \
	"ke|qiao", "ke|qiao", "yao|xiao", "xun", "dian", "hui", "hui", "gu", "qiao|que", "ji|qi", \
	"yi", "kou|qu", "hui", "duan", "yi", "xiao", "mou|wu", "wan|guan", "mu|wu", "mei", \
	"mei", "ai", "jie", "dai|du", "yu", "bi|pi", "bi", "bi", "pi", "pi", \
	"bi", "chan", "mao", "uu", "uu", "bi|pi", "mao|lie", "jia", "zhan", "sai", \
	"mu|mao", "tuo", "xun", "er", "rong", "xian", "ju", "mu", "hao", "qiu", \
	"dou|nuo", "uu", "tan", "pei", "ju", "duo", "cui|qiao|xia", "bi", "san", "san", \
	"mao", "sai|sui", "shu", "shu", "tuo", "he|ke", "jian", "ta", "san", "lv|shu", \
	"mu", "li|mao", "tong", "rong", "chang", "pu", "lu", "zhan", "sao", "zhan", \
	"meng", "lu", "qu", "die", "shi|zhi", "di|zhi", "min", "jue", "mang|meng", "qi", \
	"pie", "nai", "qi", "dao", "xian", "chuan", "fen", "ri|yang", "nei", "bin", \
	"fu", "shen", "dong", "qing", "qi|xi", "yan|yin", "xi", "hai", "yang", "an", \
	"ya", "ke", "qing", "ya", "dong", "dan", "lv", "qing", "yang", "yun", \
	"yun", "shui", "shui", "cheng|zheng", "bing", "yong", "dang", "shui", "le", "ni|mei", \
	"qiu|tun", "fan", "jiu|qiu|gui", "ding|ting", "shi|xie|zhi", "qiu", "pa|bin", "ze", "mian", "cuan", \
	"hui", "diao", "han", "cha", "zhuo|yue|que", "chuan", "wan|huan", "fan|fa", "da|tai|dai", "xi", \
	"tuo", "mang", "you|qiu", "qi", "shan|shuan", "chi|pin", "gan|han", "qian", "yu|wu", "wu|yu", \
	"xun", "si", "ru", "gong", "jiang", "che|chi|tuo", "wa|wu|yu", "tu", "jiu", "tang|shang", \
	"zhi|ji", "zhi", "qian", "mi", "gu|yu", "hong|wang", "jing", "jing", "rui|tun", "jun", \
	"hong", "tai", "fu|quan", "ji", "bian", "bian", "han|gan", "wen|men", "zhong", "fang|pang", \
	"xiong", "jue|que|xue", "hu|huang", "niu|you", "gai|qi|yi", "fen|pen", "xu", "xu", "qin", "yi|yin", \
	"wo", "yun", "yuan", "hang|kang", "yan|wei", "chen|shen|tan", "chen", "dan", "you", "dun|zhuan", \
	"hu", "huo", "qi|qie", "mu", "niu|nv", "mei|mo", "ta|da", "mian", "wu|fu|mi", "chong", \
	"pang", "bi", "sha|suo", "zhi", "pei", "pan", "zhui|zi", "za", "gou", "liu", \
	"mei|mo", "ze", "feng", "ou", "li", "guan|lun", "cang", "feng", "gui|wei", "hu", \
	"mei|mo", "mei|hui", "shu", "ju|zu", "za", "tuo|duo", "tuo|duo", "tuo|duo", "he", "zhen|li", \
	"ni|mi", "yi|chi", "fa", "fei|fu", "you", "tian", "chi|yi|zhi", "zhao", "gu", "chan|dian|tian|zhan", \
	"yan", "si", "kuang", "jiong|ying", "gou|ju", "xie|yi", "qiu|you", "die|yi", "jia", "you|zhong", \
	"quan", "bo|po", "hui", "mi|bi", "ben", "ze", "ku|zhu", "le", "ao|you", "gu", \
	"hong", "gan|han", "fa", "liu|mao", "si", "hu", "ping|peng", "ci|zi", "fa|fan|feng", "di|zhi|chi", \
	"su", "ning|zhu", "cheng", "ling", "pao", "bei|bi|bo", "li|qi|se", "si", "ni", "ju", \
	"yue|xue|sa", "zhou|zhu", "sheng", "lei", "juan|xuan", "xue|jue", "fu", "pan", "mian|min", "tai", \
	"yang", "ji", "yong", "guan", "beng|liu|pin", "xue", "long|shuang", "lu", "dan", "luo|po", \
	"xie", "bo|po", "ze", "jing", "yin", "zhou|pan", "ji|jie", "shi|ye", "hui", "hui", \
	"zai", "cheng", "yan|ye|yin", "wei", "hou", "cun|jian", "xiang|yang", "lie", "si", "ji", \
	"er", "xing", "fu", "sa|xian|xun", "zi|se|qi", "zhi", "yin", "wu", "xi|xian", "kao", \
	"zhu", "jiang", "luo", "uu", "an|yan|e", "dong|tong", "yi|ti", "mou|si", "lei", "yi", \
	"mi", "quan", "jin", "po", "wei", "xiao", "xie|yi", "hong", "xu|yi", "su|shuo", \
	"kuang", "dao|tao|yao", "jie|qie", "ju", "er", "zhou", "ru", "ping", "xuan|xun", "xiong", \
	"zhi", "guang|huang", "huan", "ming", "guo|huo", "gui|wa", "he|qia", "pai|pa", "hu|wu", "qu", \
	"liu", "yi", "jia|xia", "jing", "qian|jian", "jiang", "ao|jiao|nao", "cheng|zhen", "shi", "zhuo", \
	"ce", "peol", "hui|kuai", "ji|qi", "liu", "chan", "gun|hun", "hu|xu", "nong", "xun|yin", \
	"jin", "lie", "qiu", "wei", "zhe", "jun|xun", "gan|han", "bang|bin", "mang", "zhuo", \
	"you|di", "xi", "bo", "dou", "huan", "hong", "yi|ya", "pu", "cheng|ying", "lan", \
	"gao|ge|hao", "lang", "han", "li", "geng", "fu", "wu", "li|lian", "chun", "feng|hong", \
	"yi", "yu", "tong", "lao", "hai", "jin|qin", "jia|xia", "chong", "jiong", "mei", \
	"sui|nei", "cheng", "pei", "jian|xian", "lin|qin|sei|shen", "chu|tu|ye", "kun", "ping", "nie", "han", \
	"jing|qing", "xiao", "die|she", "ren|nian", "tu", "yong|chong", "xiao", "dian|xian|yan", "ting", "e", \
	"shu|sou|su", "tun|yun", "juan|xuan|yuan", "cen|qian|zan", "ti", "li", "shui", "si", "lei", "shui", \
	"chao|dao|shou|tao", "du", "lao", "lai", "lan|lian", "wei", "wo|guo", "yun", "huan|hui", "di", \
	"heng", "run", "jian", "zhang", "se", "fu|pou", "guan", "xing", "shou|tao", "shua|shuan", \
	"ya", "chuo", "zhang", "shi|ye", "kong|nang", "wan|yuan|wo", "han", "tuo", "dong", "he", \
	"wo", "ju", "she", "liang", "hun", "ta", "zhuo", "dian", "qie|ji", "de", \
	"juan", "zi", "xi", "xiao", "qi", "gu|hu", "guo|guan", "yan|han", "lin", "chang|tang", \
	"zhou|diao", "peng", "hao", "chang", "chu|shu", "qi|qian", "fang", "chi", "lu", "zhuo|chuo|nao", \
	"ju", "tao", "cong|shuang", "lei|li", "zhe", "peng|ping", "fei", "song", "tian", "pi|pei", \
	"dan|tan|yan", "yu|xu", "ni", "yu", "lu", "gan|han", "mi", "cheng|jing", "ling", "guan|lun", \
	"yan|yao|yin", "cui|zu", "qu", "huai", "yu", "nian|shen", "shen", "hu|biao", "chun|zhun", "hu", \
	"yuan", "lai", "gun|hun|kun", "qing", "yan", "qian|jian", "tian", "miao", "zhi", "yin", \
	"bo|po", "ben", "yuan", "wen|min", "ruo|re|luo", "fei", "qing", "yuan", "he|jie|kai|ke", "ji|qi", \
	"die|she", "yuan", "se", "lu", "qi|se|zi", "du|dou", "qi", "chan|jian|qian", "mian|sheng", "pi", \
	"xi", "yu", "yuan", "shen", "lin|qin|sen|shen", "rou", "huan", "zhu", "jian", "nuan", \
	"yu", "qiu|wu", "ting", "qu|ju", "du", "feng|fan", "zha", "bo", "ou|wo|wu", "wo|guo", \
	"di|ti", "wei", "wen|yun", "er|nuo|ruan", "die|xie|yi|zha", "ce", "wei", "he", "gang|jiang", "yan", \
	"hong|gong", "xuan", "mi", "he|jie|kai|ke", "mao", "ying", "yan", "liu|you", "hong|qing", "miao", \
	"sheng", "mei", "zai", "gun|hun", "nai", "gui", "chi", "e", "ba|pai", "mei", \
	"lan|lian", "qi", "qi", "mei", "tian", "cou", "wei", "can", "tuan|zhuan", "mian", \
	"min|xu|hui", "bo|po", "xu", "ji", "pen", "jian|qian|zan|zhan", "jian", "hu", "feng", "xiang", \
	"yi", "yin", "chen|dan|tan|zhan", "shi", "jie|xie", "zhen|zheng", "huang|kuang", "tan", "yu", "bi", \
	"min|hun", "shi", "tu", "sheng", "yong", "ju", "dong|tong", "tuan|nuan", "qiu|jiao", "qiu|jiu|jiao", \
	"qiu", "yan|yin", "tang|shang", "long", "huo", "yuan", "nan", "pan|ban", "you", "quan", \
	"hun|zhuang", "liang", "chan", "dian|xian|yan", "zhun|chun", "nie|he", "zi", "wan", "shi|ta|xi", "man|men", \
	"ying", "la", "kui|hui", "feng|hong", "jian", "xu", "lou", "wei", "gai|xie", "xia", \
	"ying", "po", "jin", "gui|yan", "tang", "yuan", "suo", "yuan", "xian|lian|nian", "yao", \
	"meng", "zhun|zhuo", "cheng", "kai|ke", "tai", "ta|da", "wa", "liu", "gou|gang", "sao", \
	"mi|ming", "zha", "shi", "yi", "lun", "ma", "bu|fu|po|pu", "wei|mei", "li", "zai", \
	"wu", "xi", "wen", "qiang", "ze", "shi", "shuo|su", "ai", "qin|zhen", "shao|sou", \
	"yun", "chou|xiu", "yin", "rong", "hun", "su", "suo|se", "ni|niao", "ta", "shi", \
	"ru", "ai", "pan", "xu|chu", "chu", "pang|peng", "weng", "cang", "mie", "ge", \
	"dian|tian|zhen", "hao|xue", "huang", "qi|xie|xi", "ci|zi", "di", "zhi", "ying|xing", "fu", "jie", \
	"gu|hua", "ge", "zi", "tao", "teng", "sui", "bi", "jiao", "hui", "gun", \
	"yao|yin", "gao|hao|ze", "long|shuang", "chi|zhi", "yan", "ni|she", "man|men", "ying", "chun", "lu|lv", \
	"jian|lan", "luan", "xiao", "bin", "han|nan|tan", "yu", "xiu", "hu", "bi", "biao", \
	"chi|zhi", "jiang", "kou", "lin|qin|sen|shen", "shang", "di", "mi", "ao", "lu", "hu|xu", \
	"hu", "you", "chan", "fan", "yong", "gun", "man|men", "qing", "yu", "biao|piao", \
	"ji", "ya", "chao", "qi|qie", "xi", "ji", "lu", "lou|lu", "long", "jin", \
	"guo", "cong|song", "lou", "zhi", "gai", "qiang", "li", "yan", "cao", "jiao", \
	"cong", "chun", "tuan|zhuan", "ou", "teng", "ye", "xi", "mi", "tang", "mo", \
	"tang|shang", "han|tan", "lan|lian", "lan", "wa", "chi|tai", "gan", "feng|peng", "xuan", "yi", \
	"man", "qi|se|zi", "mang", "kang", "luo|ta", "peng", "shu", "zhang", "zhang", "chong|zhuang", \
	"xu", "huan", "kuo|huo", "chan|jian|qian", "yan", "shuang|chuang", "liao|xiao", "cui", "ti", "yang", \
	"jiang", "zong|cong", "ying", "hong", "xin", "shu", "guan|huan", "ying", "xiao", "zong|cong", \
	"kun", "xu", "lian", "zhi", "wei", "pie|pi", "jue|yu|shu", "jiao|qiao", "po|bo", "xiang|dang", \
	"hui", "jie", "wu", "pa", "ji", "bo|pan", "wei|gui", "sou|su|xiao", "qian", "qian", \
	"xi|ya", "lu", "xi", "xun|sun", "dun", "huang|guang", "min", "run", "su", "liao|lao", \
	"zhen", "zong|cong", "yi", "zhi|zhe", "wan", "tan|shan", "dan|tan|xun|yin", "chao", "xun|yin", "kui|hui", \
	"ye", "shao", "tu|zha", "zhu", "san|sa", "hei", "bi", "shan", "chan", "chan", \
	"shu", "chong|tong|zhong", "pu", "lin", "wei", "se", "se", "cheng|deng", "jiong", "cheng|deng", \
	"hua", "ao|jiao|nao", "lao", "che", "gan|han", "cun", "hong|jing", "si", "shu|zhu", "peng", \
	"han", "yun", "liu", "hong|gong", "fu", "hao|gao", "he", "xian", "jian", "shan", \
	"xi", "ao|yu", "lu", "lan", "ning", "yu", "lin", "mian|sheng", "cao|zao", "dang", \
	"han|huan", "ze|shi", "xie", "yu", "li", "shi|cuo", "xue|xiao", "ling", "wan|man|ou", "ci|zi", \
	"yong", "hui|kuai", "can", "lian", "dian", "ye", "ao|yu", "huan|xuan", "zhen", "chan|dan|zhan", \
	"man", "gan|dan", "dan|tan", "yi", "sui", "pi", "ju", "ta", "qin", "ji|jiao", \
	"zhuo", "lian|xian", "nong", "guo|wo", "jin|qin", "pen|fen", "se", "ji|sha", "sui", "hui|huo|hun", \
	"chu", "ta", "song", "ding|ting", "se", "zhu", "lai", "bin", "lian", "mi|ni", \
	"shi|ta|xi", "shu", "mi", "ni|ning", "ying", "ying", "meng", "jin", "qi", "bi|pi", \
	"ji|qi", "hao", "er|nuan|ruan", "cui|zui", "wo", "chao|dao|shou|tao", "yin", "yin", "dui", "ci", \
	"huo|hu", "qing|jing", "jian|lan", "jun|xun", "kai|ai", "pu", "zhuo|zhao", "wei", "bin|bang", "gu", \
	"qian", "ying", "bin", "kuo", "fei", "cang", "me", "jian|zan", "wei|dui", "luo|po", \
	"cuan|qian|za|zan", "lu", "li", "you", "yang", "lu", "si", "zhi", "jiong|ying", "du|dou", \
	"wang", "hui", "xie", "pan", "chen|pan|shen", "biao", "chan", "mie|mo", "liu", "jian", \
	"pu|bao", "se", "cheng", "gu", "bin", "huo", "xian", "lu", "qin", "han", \
	"ying", "rong", "li", "jing|cheng", "xiao", "ying", "sui", "wei|dui", "xie", "huai|wai", \
	"xue", "zhu", "long|shuang", "lai", "dui", "fan", "hu", "lai", "shu", "ling|lian", \
	"ying", "mi|ni", "ji", "lian", "jian|zun", "ying", "fen", "lin", "yi", "jian", \
	"yao|yue", "chan", "dai", "rang|nang", "jian", "lan", "fan", "shuang", "yuan", "jiao|ze|zhuo", \
	"feng", "she|ni", "lei", "lan", "cong", "qu", "yong", "qian", "fa", "guan|huan", \
	"jue", "yan", "hao", "ying", "li|sa|xian", "zan|cuan", "luan", "yan", "li", "mi", \
	"shan", "han|nan|tan", "dang|tang", "jiao", "chan", "ying", "hao", "ba", "zhu", "lan", \
	"lan", "nang", "wan", "luan", "quan|xun", "xian", "yan", "gan", "yan", "yu", \
	"huo", "huo", "mie", "guang", "deng|ding", "hui", "xiao", "xiao", "hui", "hong", \
	"ling", "zao", "zhuan", "jiu", "zha|yu", "xie", "chi", "zhuo", "zai", "zai", \
	"can", "yang", "qi", "zhong", "fen|ben", "niu", "jiong|gui", "wen", "pu", "yi", \
	"lu", "chui", "pi", "kai", "pan", "tan|yan", "yan|kai", "feng|pang", "mu", "chao", \
	"liao", "que|gui", "hang|kang", "dun|tun", "guang", "xin", "zhi", "guang", "guang", "wei", \
	"qiang", "bian", "da", "xia", "zheng", "zhu", "ke", "zhao", "fu", "ba", \
	"xie", "xie", "ling", "zhuo|chu", "xuan", "ju", "tan", "pao|bao", "jiong", "pao|fou", \
	"tai", "tai", "bing", "yang", "tong", "qian|shan", "zhu", "zha", "dian", "ha|wei", \
	"shi", "lan|lian", "chi", "huang", "zhou", "hu", "luo|shuo|yue", "lan", "jing|ting", "jiao|yao", \
	"xu", "heng", "quan", "lie", "huan", "yang", "xiu|xiao", "xiu", "xian", "yin", \
	"wu|ya", "zhou", "yao", "shi", "wei", "tong|dong", "xue", "zai", "kai", "hong", \
	"lao|luo", "xia", "chong|zhu", "xuan|hui", "zheng", "po", "yan|yin", "ai|hui", "guang", "che", \
	"hui", "kao", "chen", "fan", "shao", "ye", "hui", "uu", "dang|tang", "jin", \
	"re", "lie", "xi", "fu", "jiong", "che|xie", "pu", "jing|ting", "zhuo", "ting", \
	"wan", "hai", "peng", "lang", "yan|shan", "xu", "feng", "chi", "rong", "hu", \
	"xi", "shu", "he|huo", "hun|xun", "ku|kao", "juan|jue", "xiao", "xi", "yan|yi", "han", \
	"zhuang", "jun|qu", "di", "che|xie", "ji|qi", "wu", "uu", "lv", "han", "yan", \
	"huan", "men", "ju", "tao|dao", "bei", "fen", "lin", "kun", "hun", "tun|tui", \
	"xi", "cui", "wu|mo", "hong", "chao|ju", "fu", "wo|ai", "jiao|qiao", "cong", "feng", \
	"ping", "qiong", "ruo", "xi|yi", "qiong", "xin", "zhuo|chao", "yan", "yan|yi", "yi", \
	"jiao|qiao", "yu", "gang", "ran", "pi", "ying|xiong|gu", "wang", "sheng", "chang|gua", "shao", \
	"ying|xiong", "nian|ne", "geng", "qu|kuo", "chen", "he", "kui", "zhong", "duan", "xia", \
	"hui|yun|xun", "feng", "lian|lan", "xuan", "xing", "huang", "jiao", "jian", "bi", "ying", \
	"zhu", "wei|hui", "tuan", "shan|qian", "xi|yi", "nuan|xuan", "nuan", "chan", "yan", "jiong", \
	"jiong", "yu", "mei", "sha", "wei", "ye|zha", "jin", "qiong", "rou", "mei", \
	"huan", "xiu|xu", "zhao", "wei|yu", "fan", "qiu", "sui", "yang", "lie", "zhu", \
	"jie", "sao|zao", "gua", "bao", "hu", "wen|yun", "nan", "shi", "liang|huo", "bian", \
	"gou", "tui", "tang", "chao", "shan", "en|yun", "bo", "huang|ye", "xie", "xi", \
	"wu", "xi", "yun", "he", "he|xiao", "xi|yi", "yun", "xiong", "xiong|nai", "shan", \
	"qiong", "yao", "xun", "mi", "qian|lian", "ying|xing", "wu", "rong", "gong", "yan", \
	"qiang", "liu", "xi|yi", "bi", "biao", "cong|zong", "lu|ao", "jian", "shu|shou", "yi", \
	"lou", "feng|peng", "cui|sui", "yi", "tong|teng", "jue", "zong", "yun|yu", "hu", "yi", \
	"zhi", "ao", "wei", "liu", "han|ran", "ou", "re", "jiong", "man", "kun", \
	"shang", "cuan", "zeng", "jian", "xi", "xi", "xi", "yi", "xiao", "chi", \
	"huang", "dan|chan", "ye", "tan|xun", "ran", "yan", "xun", "qiao|xiao", "jun", "deng", \
	"dun|tun", "shen", "jiao|qiao|jue|zhuo", "fen|ben", "si|xi", "liao", "yu", "lin", "tong|dong|jiong", "shao", \
	"fen", "fan|fen", "yan", "xun", "lan", "mei", "tang|dang", "yi", "jiong", "men", \
	"jing", "uu", "cuo|ying", "yu", "yi", "xue", "lan", "lie|tai", "sao|zao", "can", \
	"sui", "xi", "que", "cong|zong", "lian|qian", "hui", "kuo|zhu", "xie", "ling", "yu|wei", \
	"yi", "xie", "zhao", "hui", "da", "nung", "bing", "ru|ruan", "bing|xian", "xiao|he", \
	"xun", "jin", "chou", "tao|dao", "yao|shuo", "he", "lan", "biao", "rong", "li|lie", \
	"mo", "bao|bo", "ruo", "lv", "la|lie", "ao", "xun", "huang|kuang", "shuo|luo", "liao", \
	"li", "lu", "jue", "liao", "yan|xun", "xi", "xie", "long", "ye", "can", \
	"rang", "yue", "lan", "cong", "jue", "chong|tong", "guan", "qu|ju", "che", "mi", \
	"tang", "lan", "kuo|zhu", "lan", "ling", "cuan", "yu", "zhua|zhao", "zhua", "pa", \
	"zheng", "pao", "cheng|chen", "yuan", "ai", "wei", "han", "jue", "jue", "fu", \
	"ye", "ba", "die", "ye", "xiao|yao", "zu", "shuang", "er|mi", "pan|qiang", "chuang", \
	"ke", "zang", "die", "qiang", "yong", "qiang", "pan|pian", "ban", "pan", "chao", \
	"jian", "pai", "du", "chuang", "yu", "zha", "bian|mian", "die", "bang|pang", "bo", \
	"chuang", "you", "yong|you", "du", "ya", "cheng", "niu", "niu", "pin", "le|jiu", \
	"mou|mu", "ta|tuo", "mu", "lao|lou", "ren", "mang", "fang", "mao", "mu", "gang", \
	"wu", "yan", "ge|qiu", "bei", "si", "jian", "gu", "you|chou", "ke|ge", "sheng", \
	"mu", "di|zhai", "qian", "quan", "quan", "zi", "te", "suo|xi", "mang", "keng", \
	"qian", "wu", "gu", "xi", "li", "li", "pou", "ji|yi", "gang", "zhi|te", \
	"ben", "quan", "chun", "du", "ju", "jia", "jian|qian", "feng", "pian", "ke", \
	"ju", "kao", "chu", "xi", "bei", "luo", "jie", "ma", "san", "wei", \
	"li|mao", "dun", "tong", "qiao", "jiang", "xi", "li", "du", "lie", "bai|pai", \
	"piao|pao", "bao|bo", "xi|suo", "chou", "wei", "kui|rao", "chou", "quan", "quan", "ba|quan", \
	"fan", "qiu", "ji", "chai", "bao|zhuo", "han|an", "ge|he", "zhuang", "guang", "ma", \
	"you", "kang|gang", "fei|pei|bo", "hou", "ya", "yin", "huan|fan", "zhuang", "yun", "jue|kuang", \
	"niu|nv", "di|ti", "kuang", "zhong", "mu", "bei", "pi", "ju", "quan|yi|chi", "sheng|xing", \
	"pao", "xia", "yi|tuo", "hu", "ling", "fei", "pi", "ni", "yao", "you", \
	"gou", "xue", "ju", "dan", "bo", "ku", "xian", "ning", "huan|xuan", "hen|ken|yan", \
	"jiao|xiao", "he|mo", "zhao", "ji|jie", "xun", "shan", "shi|ta", "rong", "shou", "tong", \
	"lao|dong", "du", "xia", "shi", "kuai", "zheng", "yu", "sun", "yu", "bi", \
	"mang|zhuo", "xi|shi", "juan", "li", "xia", "yin", "jun|suan", "hang|lang", "bei", "zhi", \
	"yan", "sha", "li", "han", "xian", "jing", "pai", "fei", "xiao", "pi|bai", \
	"qi", "ni", "biao", "yin", "lai", "lie|que|xi", "jian", "qiang", "kun", "yan", \
	"guo|luo", "zong", "mi", "chang", "ji|wei|yi", "zhi", "zheng", "wei|ya", "meng", "cai", \
	"cu", "she", "lie", "ceon", "luo", "hu", "zong", "fui", "wei", "feng", \
	"wo", "yuan", "xing", "zhu", "mao|miao", "wei", "chuan|shan", "xian", "tuan", "jia|ya", \
	"nao", "xie|ge", "jia", "hou", "bian|pian", "you|yao", "you", "mei", "cha|zha", "yao", \
	"sun", "bo|po", "ming", "hua", "yuan", "sou", "ma", "yuan", "dai", "yu", \
	"shi", "hao", "qiang", "yi", "zhen", "cang", "gao|hao", "man", "jing", "jiang", \
	"mao|mu", "zhang", "chan", "ao", "ao", "gao|hao", "cui|suo", "ben|fen", "jue", "bi", \
	"bi", "huang", "pu|bu", "lin", "xu|yu", "tong|zhuang", "xiao|yao", "lao|liao", "que|xi|shuo", "xiao", \
	"shou", "dun|du", "jiao", "ge|lie|xie", "xuan|juan", "du", "hui", "kuai|hua", "xian", "ha|jie|xie", \
	"ta", "xian", "mi|xun", "ning", "bian|pian", "huo", "ru|nou", "meng", "lie", "you|nao", \
	"guang|jing", "shou", "lu", "ta", "xian|suo", "mi", "rang", "huan|quan", "nao", "e|luo", \
	"xian", "qi", "jue", "xuan", "miao|yao", "zi|ci|xuan", "shuai|lv", "lu", "yu", "su", \
	"wang|yu", "qiu", "ga", "ding", "le", "ba", "ji", "hong", "di", "chuan", \
	"gan", "jiu", "yu", "qi", "yu", "yang|chang", "ma", "hong", "wu", "fu", \
	"wen|min", "jie", "ya", "fen|bin", "men", "bang", "yue", "jue", "yun|men", "jue", \
	"wan", "yin|jian", "mei", "dan", "pin", "wei", "huan", "xian", "qiang", "ling", \
	"dai", "yi", "gan|an", "ping", "dian", "fu", "xian|xuan", "xi", "bo", "ci|cou", \
	"gou", "jia", "shao", "po", "ci", "ke", "ran", "sheng", "shen", "yi|tai", \
	"zu|ju", "jia|ka", "min", "shan", "liu", "bi", "zhen", "zhen", "jue", "fa", \
	"long", "jin", "jiao", "jian", "li", "guang", "xian", "zhou", "gong", "yan", \
	"xiu", "yang", "xu", "li|luo", "su", "zhu", "qin", "yin|ken", "xun", "bao", \
	"er", "xiang", "yao", "xia", "hang|heng", "gui", "chong", "xu", "ban", "pei", \
	"lao", "dang", "ying", "hui|hun", "wen", "e", "cheng|ting", "di|ti", "wu", "wu", \
	"cheng", "jun", "mei", "bei", "ting", "xian", "chu", "han", "xuan|qiong", "yan", \
	"qiu", "xuan", "lang", "li", "xiu", "fu", "liu", "ya", "xi", "ling", \
	"li", "jin", "lian", "suo", "suo", "feng", "wan", "dian", "bing|pin", "zhan", \
	"cui|se", "min", "yu", "ju", "chen", "lai", "min", "sheng|wang", "wei|yu", "tian", \
	"chu", "zhuo|zuo", "pei|beng", "cheng", "hu", "qi", "e", "kun", "chang", "qi", \
	"beng", "wan", "lu", "cong", "guan|gun", "yan", "diao", "bei|fei|pei", "lin", "qin", \
	"pi", "pa", "qiang", "zhuo", "qin", "fa", "jin", "qiong", "du", "jie", \
	"hui|hun", "yu", "mao|q", "mei", "chun", "xuan", "ti", "xing", "dai", "rou", \
	"min", "jian", "wei", "ruan", "huan", "jie|xie", "chuan", "jian", "zhuan", "yang|chang", \
	"lian", "quan", "xia", "duan", "huan|yuan", "ye|ya", "nao", "hu", "ying", "yu", \
	"huang", "rui", "se", "liu", "shi", "rong", "suo", "yao", "wen", "wu", \
	"zhen", "jin", "ying", "ma", "tao", "liu", "tang", "li", "lang", "gui", \
	"tian|zhen", "cang|qiang|cheng", "cuo", "jue", "zhao", "yao", "ai", "bin|pian", "tu|shu", "chang", \
	"kun", "zhuan", "cong", "jin", "yi", "cui", "cong", "ji|qi", "li", "jing", \
	"zao|suo", "qiu", "xuan", "ao", "lian", "men", "zhang", "yin", "hua|ye", "ying", \
	"wei", "lu", "wu", "deng", "xiu", "zeng", "xun", "qu", "dang", "lin", \
	"liao", "jue|qiong", "su", "huang", "gui", "pu", "jing", "fan", "jin", "liu", \
	"ji", "hui", "jing", "ai", "bi", "can", "qu", "zao", "dang", "jiao", \
	"gun|guan", "tan", "kuai|hui", "huan", "se", "sui", "tian", "chu", "yu", "jin", \
	"fu|lu", "bin|pian", "shu", "wen", "zui", "lan", "xi", "ji|zi", "xuan", "ruan", \
	"wo", "gai", "lei", "du", "li", "zhi", "rou", "li", "zan", "qiong|xuan", \
	"ti", "gui", "sui", "la", "long", "lu", "li", "zan", "lan", "ying", \
	"mi|xi", "xiang", "qiong|wei", "guan", "dao", "zan", "huan|ye|ya", "gua", "bo", "die", \
	"pao|bo", "gu|hu|huo", "hu|zhi", "piao", "ban", "rang", "li", "wa", "shiwa", "hong|xiang", \
	"qianwa", "ban", "pen", "fang", "dan", "weng", "ou", "fenwa", "miliklanm", "wa", \
	"hu", "ling", "yi", "ping", "ci", "baiwa", "juan", "chang", "chi", "liwa", \
	"dang", "wa|meng", "bu", "zhui", "ping", "bian", "zhou", "juan|zhen", "liwa", "ci", \
	"ying", "qi", "xian", "lou", "di", "ou", "meng", "chuan|zhuan", "beng", "lin", \
	"zeng", "wu", "pi", "dan", "weng", "ying", "yan", "gan|han", "dai", "shen", \
	"tian", "tian", "han", "chang", "sheng", "qing", "shen", "chan", "chan", "rui", \
	"sheng", "su", "shen", "yong", "shuai", "lu", "fu|pu", "dong|yong", "beng|qi", "beng|feng", \
	"ning", "tian", "yao|you", "jia", "shen", "you|zha", "dian", "fu", "nan", "dian|sheng|tian|ying", \
	"ping", "ting|ding", "hua", "ding", "quan|zhen", "zi|zai", "meng|mang", "bi", "bi", "jiu|liu", \
	"sun", "liu", "chang", "mu", "tian|yun", "fan", "fu", "geng", "tian", "jie", \
	"jie", "quan", "wei", "fu|bi", "tian", "mu", "tap", "pan", "jiang", "wa", \
	"fu|da", "nan", "liu", "ben", "zhen", "chu|xu", "mu", "mu", "ji|ce", "zi|zai|tian", \
	"gai", "bi", "da", "zhi|shi|chou", "lue", "qi", "lue", "fan|pan", "yi", "fan|pan", \
	"hua", "she|yu", "she|yu", "mu", "jun", "yi", "liu", "she", "die", "chou", \
	"hua", "dang", "zhui", "ji|qi", "wan", "jiang", "cheng", "chang", "tun|tuan", "lei", \
	"ji", "cha|chai", "liu", "die", "tuan", "lin", "jiang", "jiang", "chou", "pi", \
	"die", "die", "pi|ya|shu", "jie|qie", "dan", "shu", "shu", "di|zhi", "ning|yi", "ne", \
	"nai", "ding|ne", "bi", "jie", "liao", "gang", "ge|yi", "jiu", "zhou", "xia", \
	"shan", "xu", "nue|yao", "li|lai", "yang", "chen", "you", "ba", "jie", "jue|xue", \
	"qi", "xia|ya", "cui", "bi", "yi", "li", "zong", "chuang", "feng", "zhu", \
	"pao", "pi", "gan", "ke|qia", "ci|ji|zhai|zi", "xue", "zhi", "dan|da", "chen|zhen", "bian|fa", \
	"zhi", "teng", "ju", "ji", "fei", "ju", "shan", "jia", "xuan", "zha", \
	"bing", "nei|ni", "zheng", "yong", "jing", "quan", "chong|teng", "tong", "yi", "jie", \
	"you|wei", "hui", "shi|tan", "yang", "chi", "zhi", "gen|hen", "ya", "mei", "dou", \
	"jing", "xiao", "tong", "tu", "mang", "pi", "xiao", "suan", "pu", "li", \
	"zhi", "cuo", "duo", "pi|wu", "sha", "lao", "shou", "huan", "xian", "yi", \
	"peng|beng", "zhang", "guan", "tan", "fei", "ma", "ma|lin", "chi", "ji", "dian|tian", \
	"an|ye", "chi", "bi", "bi|pi", "min", "gu", "dui", "ke", "wei", "yu", \
	"cui", "ya", "zhu", "cu", "dan", "shen", "zhong", "chi|zhi", "yu", "hou", \
	"feng", "la", "dang|yang", "chen", "tu", "yu", "guo", "wen", "huan", "ku", \
	"xia|jia", "yin", "yi", "lou", "sao", "jue", "chi", "xi", "guan", "yi", \
	"wen|wo|yun", "ji", "chuang", "ban", "hui|lei", "liu", "chai|cuo", "shou", "nue|yao", "dian|chen", \
	"da", "bie", "tan", "zhang", "biao", "shen", "cu", "luo", "yi", "zong", \
	"chou|lu", "zhang", "ji|zhai", "sou", "se", "que", "diao", "lou", "lou|lv", "mo", \
	"qin", "yin", "ying", "huang", "fu", "liao|shuo", "long", "jiao|qiao", "liu", "lao", \
	"xian", "fei", "dan|tan", "yin", "he", "ai", "ban", "xian", "guan", "wei|gui", \
	"nong", "yu", "wei", "yi", "yong", "pi", "lei", "li", "shu", "dan", \
	"lin|bing", "dian", "lin|bing", "lai|la", "bie", "ji", "chi", "yang", "xuan", "jie", \
	"zheng", "me", "li", "huo", "lai|la", "ji", "dian", "xuan", "ying", "yin", \
	"qu", "yong", "tan", "dian", "luo", "luan", "luan", "bo", "uu", "gui", \
	"ba", "fa", "de|deng", "fa|bo", "bai|bo", "bai|bo|mo", "qie", "bi|ji", "zao", "zao", \
	"mao", "de|di", "ba|pa", "jie", "huang|wang", "gui", "ci", "ling", "gao|yao", "mo", \
	"ji", "jiao", "peng", "gao|yao", "ai", "e", "hao|hui", "han", "bi", "huan|wan", \
	"chou", "qian", "xi", "ai", "po|xiao", "hao", "huang", "hao", "ze", "cui", \
	"hao", "xiao", "ye", "pan|po", "hao", "jiao", "ai", "xing", "huang", "li|luo|bo", \
	"piao", "he", "jiao", "pi", "gan", "pao", "zhou", "jun", "qiu", "cun", \
	"que", "zha", "gu", "jun", "jun", "zhou", "cu|zha", "uu", "zhao|zhan|dan", "du", \
	"min", "qi", "ying", "yu", "bei", "zhao", "chong|zhong", "pen", "he", "ying", \
	"he", "yi", "bo", "wan", "he|ke", "ang", "zhan", "yan", "jian", "an|he", \
	"yu", "kui", "fan", "gai|ge", "dao", "pan", "fu", "qiu", "sheng|cheng", "dao", \
	"lu", "zhan", "meng|ming", "li", "jin", "xu", "jian|kan", "pan|xuan", "guan", "an", \
	"lei|lu|lv", "xu", "chou|zhou", "dang", "an", "gu", "li", "mu", "cheng|ding", "gan", \
	"xu", "mang", "mang|wang", "zhi", "qi", "yuan", "tian|xian", "xiang", "dun|zhun", "xin", \
	"pan|xi", "fen|pan", "feng", "dun|yun", "min", "ming", "sheng|xing", "shi", "hun|yun", "mian", \
	"pan", "fang", "miao", "chen|dan", "mei", "mao|mei", "kan", "xian", "kou", "shi", \
	"yang|ying", "zheng", "ao|yao", "shen", "huo", "da", "zhen", "kuang", "xu|ju", "shen", \
	"yi|chi", "sheng", "mei", "mo|mie", "zhu", "zhen", "zhen", "mian|min", "shi", "yuan", \
	"die|chou", "ni", "zi", "zi", "chao", "zha", "huan|juan|xuan", "bing|fang", "pan|mi", "long", \
	"gui|sui", "tong", "mi", "zhi|die", "di", "ne", "ming", "xun|shun|xuan", "chi", "kuang", \
	"juan", "mou", "zhen", "tiao", "yang", "wen|yan", "mo|mi", "zhong", "mo", "zhe|zhao|zhuo", \
	"zheng", "mei", "juan|suo", "shao|xiao|qiao", "han", "huan", "di|ti", "cheng", "cuo|zhuai", "juan", \
	"e", "man", "xian", "xi", "kun", "lai", "jian", "shan", "tian", "gun|huan", \
	"wan", "leng", "shi", "qiong", "li|lie", "ya", "jing", "zheng", "li", "lai", \
	"sui|zui", "juan", "shui", "hui|sui", "du", "pi", "bi|pi", "mu", "hun", "ni", \
	"lu", "gao|yi", "jie|she", "cai", "zhou", "yu", "hun", "ma", "xia", "xing", \
	"hui", "gun", "zai", "chun", "jian", "mei", "du", "hou", "xuan", "tian", \
	"ji|kui", "gao|hao", "rui", "mao|wu", "xu", "fa", "wo", "miao", "chou", "kui|gui", \
	"mi", "weng", "kou|ji", "dang", "chen|shen|tian", "ke", "sou", "xia", "huan|qiong", "mo", \
	"meng|mian|ming", "man", "fen", "ze", "zhang", "yi", "diao|dou", "kou", "mo", "shun", \
	"cong", "lou|lv", "chi", "man|men", "piao", "cheng|zheng", "gui", "mang|meng", "huan|wan", "shun", \
	"bi|pie", "xi", "qiao", "pu", "zhu", "deng", "shen", "shun", "liao", "che", \
	"jian|xian", "kan", "ye", "xu|xue", "tong", "wu|mou|mi", "lian|lin", "gui|kui", "jian|xian", "ye", \
	"ai", "hui", "zhan", "jian", "gu", "zhao", "ju|qu", "wei|mei", "chou", "sao", \
	"ning|cheng", "xun", "yao", "huo|yue", "meng", "mian", "pin", "mian", "lei", "kuang|guo", \
	"jue", "xuan", "mian", "huo", "lu", "meng", "long", "guan|quan", "man", "li|xi", \
	"chu", "tang", "kan", "zhu", "mao", "jin|qin|guan", "jin|qin|guan", "yu|xu|jue", "shuo", "ze|zhuo", \
	"jue", "shi", "xian|yi", "shen", "zhi", "hou", "shen", "ying", "ju", "zhou", \
	"jiao", "cuo", "duan", "ai", "jiao", "zeng", "yue", "ba", "shi|dan", "ding", \
	"qi|diao", "ji", "zi", "gan|han", "wu", "da|zhe", "ku|qia", "gang|kong", "xi", "fan", \
	"kuang", "dang", "ma", "sha", "dan", "jue", "li", "fu", "min|wen", "e", \
	"hua|xu", "kang", "zhi", "qi|qie", "kan", "jie", "fen|pin", "e", "ya", "pi", \
	"zhe", "xing|yan", "sui", "zhuan", "che", "dun", "wa", "yan", "jin", "feng", \
	"fa|ge|jie", "mo", "zha|zuo", "zu|ju", "yu", "ke|luo", "tuo", "tuo", "di", "zhai", \
	"zhen", "e", "fei|fu", "mu", "zhu", "la|li", "bian", "nu", "ping", "peng|ping", \
	"ling", "bao|pao|pu", "le", "po", "bo|e", "po", "shen", "za", "ai", "li", \
	"long", "tong", "yong", "li", "kuang", "chu", "keng", "quan", "zhu", "guang|kuang", \
	"gui|he", "e", "nao", "qia", "lu", "wei|gui", "ai", "ge|luo", "ken|yin|xian", "keng|xing", \
	"xing|yan", "dong|tong", "peng|ping", "xi", "lao", "hong", "shuo", "xia", "qiao", "qing", \
	"ai|wei", "qiao", "ji|ce", "keng|qing", "qiao|xiao", "ku|ke|que", "chan", "lang", "hong", "yu", \
	"xiao", "xia", "mang|bang", "long|luo", "yong|tong", "che", "che", "e|wo|yi", "chu|liu", "geng|ying", \
	"mang", "que", "yan", "sha", "kun", "yu|gu", "ceok", "hua", "lu", "cen|chen", \
	"jian", "nue", "song", "zhuo", "keng", "peng", "yan", "chui|zhui|duo", "kong", "cheng", \
	"qi", "zong|cong", "qing", "lin", "jun", "bo|pan", "ding", "min", "diao", "zhan|jian", \
	"he", "lu|liu", "ai", "sui", "xi|que", "leng", "bei", "yin", "dui", "wu", \
	"qi", "lun", "wan", "dian", "gang|nao", "bei", "qi", "chen", "ruan", "yan", \
	"die|she", "ding", "zhou", "tuo", "jie|ya", "ying", "bian", "ke", "bi", "wei", \
	"shuo", "zhen|an", "duan", "xia", "dang", "ti|di", "nao", "peng", "jian|xian", "di", \
	"tan", "cha", "tian", "qi", "dun", "feng", "xuan", "que", "que|qiao", "ma", \
	"gong", "nian", "xie|su", "e", "ci", "liu", "ti|si", "tang", "bang|pang", "ke|hua", \
	"pi", "kui|wei", "sang", "lei", "cuo", "tian", "xia|qia", "qi", "lian", "pan", \
	"ai|wei", "yun", "chui|dui", "zhe", "ke", "la", "pak", "yao", "gun", "tuan|tuo|zhuan", \
	"chan", "qi", "ao|qiao", "peng", "liu", "lu", "kan", "chuang", "ca|chen", "yin", \
	"lei", "piao", "qi", "mo", "qi|zhu", "cui", "zong", "qing", "chuo", "lun", \
	"ji", "shan", "lao|luo", "qu", "zeng", "deng", "jian", "xi", "lin|ling", "ding", \
	"dian", "huang|kuang", "pan|bo", "ji|she|za", "ao|qiao", "di", "li", "jian", "jiao", "xi", \
	"zhang", "qiao", "dun", "jian|xian", "yu", "zhui", "he|qiao", "huo|ke", "ze", "lei", \
	"jie", "chu", "ye", "que|hu", "dang", "yi", "jiang", "pi", "pi", "yu", \
	"pin", "e|qi", "ai|yi", "ke", "jian", "yu", "ruan", "meng", "pao", "ci", \
	"bo", "yang", "ma", "ca", "xian|xin", "kuang", "lei", "lei", "zhi", "li", \
	"li|luo", "fan", "que", "pao", "ying", "li", "long", "long", "mo", "bo", \
	"shuang", "guan", "lan|jian", "ca", "yan", "qi|shi|zhi", "pianpang|shi", "li", "reng", "she", \
	"yue", "si", "qi|zhi", "ta", "ma", "xie", "yao", "xian", "zhi|qi|chi", "gui|qi", \
	"zhi", "fang|beng", "dui", "chong|zhong", "uu", "yi", "shi", "you", "zhi", "tiao", \
	"fei|fu", "fu", "mi", "jie|zu", "qi|zhi", "suan", "mei", "zuo", "qu", "hu", \
	"chu|zhou|zhu", "shen", "sui", "ci|si", "chai", "mi|ni", "lv", "yu", "xiang", "wu", \
	"tiao", "piao", "zhu", "gui", "xia", "zhi", "ji|zhai", "gao", "zhen", "gao", \
	"shui|lei", "jin", "shen", "gai", "kun", "di", "dao", "huo", "tao", "qi", \
	"gu", "guan", "zui", "ling", "lu", "bing", "jin", "dao", "zhi", "lu", \
	"chan|shan", "bi|pi", "chu|zhe", "hui", "you|chao", "xi", "yin", "zi", "huo", "zhen", \
	"fu", "yuan", "xu|wu", "xian", "yang|shang", "zhi|ti", "yi", "mei", "si", "di", \
	"bei", "zhuo", "zhen", "yong", "ji", "gao", "tang", "si", "ma", "ta", \
	"fu", "xuan", "qi", "yu", "xi", "ji|qi", "si", "chan|shan", "dan", "gui", \
	"sui", "li", "nong", "mi|ni", "dao", "li", "rang", "yue", "ti|zhi", "zan", \
	"lei", "rou", "yu", "yu|ou", "chi|li", "xie", "qin", "he", "tu", "xiu", \
	"si", "ren", "tu", "zi", "na|cha", "gan", "zhi|yi", "xian", "bing", "nian", \
	"qiu", "qiu", "zhong|chong", "fen", "mao|hao", "yun", "ke", "miao", "zhi", "jing", \
	"bi", "zhi", "yu", "mi|bi", "ku", "ban", "pi", "ni", "li", "you", \
	"ju|zu", "pi", "bo", "ling", "mo", "cheng|ping", "nian", "qin", "yang", "zuo", \
	"zhi", "zhi", "shu", "ju", "zi", "kuo|huo", "ji|zhi", "cheng|chen", "tong", "zhi|shi", \
	"kuo|huo", "he|huo|ge", "yin", "zi", "zhi", "ji|jie", "ren", "du", "chi|yi", "zhu", \
	"hui", "nong", "bu|pu|fu", "xi", "gao", "lang", "fu", "ze|xun", "shui|tuan|tui|tuo", "lv", \
	"kun", "gan", "jing", "ti", "cheng", "tu|shu", "shao", "shui|tuan|tui|tuo", "ya", "lun", \
	"lu", "gu", "zuo", "ren", "zhun", "bang", "bai", "qi|ji", "zhi", "zhi", \
	"kun", "ling|leng", "peng", "hua|ke", "bing|lin", "chou|diao|tiao", "zu|zui", "yu", "su", "lue|su", \
	"uu", "yi", "qie|xi", "bian", "ji", "fu", "bi|pi", "nuo", "jie", "chong|zhong", \
	"zong", "xu", "cheng|chen", "dao", "wen", "xian|lian", "zi|jiu", "yu", "ji|ze", "xu", \
	"bian|zhen", "zhi", "dao", "jia", "ji|qi", "gao|kao", "gao", "gu|yu", "rong", "sui", \
	"rong", "ji", "kang", "mu", "can|cen|shan", "mi|men", "ti|zhi", "ji", "lu|jiu", "su", \
	"ji", "ying", "wen", "qiu", "se", "kweok", "yi", "huang", "qie", "ji", \
	"sui", "rao|xiao", "pu", "jiao", "bo|zhuo", "zhong|tong", "zui", "lu|lv", "sui", "nong", \
	"se", "hui", "rang", "nuo", "yu", "pin", "ji|zi", "tui", "wen", "chen|cheng", \
	"huo|hu", "kuang", "lv", "biao|pao", "se", "rang|reng", "zhuo|jue", "li", "zan|cuan", "jue|xue", \
	"wa|ya", "jiu", "qiong", "xi", "kong|qiong", "kong", "yu", "shen", "jing", "yao", \
	"chuan|yuan", "tun|zhun", "tu", "lao", "qie", "zhai", "yao", "bian", "bao", "yao", \
	"bing", "wa", "ku|zhu", "jiao|liao|liu|pao", "qiao", "diao", "wu", "wa|gui", "yao", "die|zhi", \
	"chuang", "yao", "tiao|yao", "jiao|zao", "chuang|cong", "jiong", "xiao", "cheng", "kou", "cuan", \
	"wo", "dan", "ku", "ke", "zhuo", "huo|xu", "su", "guan", "kui", "dou", \
	"zhuo", "yin|xun", "wo", "wa", "ya|ye", "dou|yu", "lou|ju", "qiong", "qiao|yao", "yao", \
	"tiao", "chao", "yu", "tian", "diao", "lou|ju", "liao", "xi", "wu", "kui", \
	"chuang", "ke|zhao", "kuan", "cuan|kuan", "long", "cheng", "cui", "liao", "zao", "cuan", \
	"qiao", "qiong", "dou|du", "zao", "long", "qie", "li|wei", "chu", "shi", "fu", \
	"qian", "chu", "hong", "qi", "hao", "sheng", "fen", "shu", "miao", "qu|kou", \
	"zhan", "zhu", "ling", "long|neng", "bing", "jing", "jing", "zhang", "bai", "si", \
	"jun", "hong", "tong|zhong", "song", "zhen|jing", "diao", "yi", "shu", "jing", "qu", \
	"jie", "ping", "duan", "li", "zhuan", "ceng", "deng", "cun", "wai", "jing", \
	"kan", "jing", "zhu", "du|zhu", "le|jin", "peng", "yu", "chi", "gan", "mang", \
	"du|zhu", "wan", "du", "ji", "jiao", "ba", "suan", "ji", "qin", "zhao", \
	"sun", "ya", "rui|zhui", "yuan", "hu|wen|wu", "hang", "xiao", "cen|jin|han", "bi|pi", "bi", \
	"jian|xian", "yi", "dong", "shan", "sheng", "xia|na|da", "di", "zhu", "na", "chi", \
	"gu", "li", "qie", "min", "bao", "shao|tiao", "si", "fu", "ce|shan", "ben", \
	"ba|bo|fa|pei", "da", "zi", "di", "ling", "ze|zuo", "nu", "fu|fei", "gou", "fan", \
	"jia", "gan", "fan", "shi", "mao", "po", "shi|xiao", "jian", "qiong", "long", \
	"min", "bian", "luo", "gui", "qu", "chi", "yin", "yao", "xian", "bi", \
	"qiong", "kuo", "deng", "jiao", "jin|qian", "quan", "sun|xun|yun", "ru", "fa", "kuang", \
	"zhu", "dong|tong", "ji", "da", "hang", "ce", "zhong", "kou", "lai", "bi", \
	"shai|shi", "dang", "zheng", "ce", "fu", "yun|jun", "tu", "pa", "li", "lang", \
	"ju", "guan", "jian|xian", "han", "tong|yong", "xia", "zhi", "cheng", "suan", "shi", \
	"zhu", "zuo", "xiao", "shao", "ting", "jia|ce", "yan", "gao", "kuai", "gan", \
	"chou|tao", "kuang", "gang", "xun|yun", "o", "qian", "xiao", "jian", "bu|pou|fu", "lai", \
	"bei|bi|zou", "bei|pai", "bi", "bi|pi", "ge", "tai|chi", "guai|dai", "yu", "jian", "dao|zhao", \
	"gu", "hu|chi", "zheng", "jing|qing", "sha|zha", "zhou", "lu", "bo", "ji", "lin", \
	"suan", "jun|qun", "fu", "zha", "gu", "kong", "qian", "quan|qian", "jun", "chui|zhui", \
	"guan", "wan|yuan", "ce", "zu", "bo|po", "ze|zhai", "qie", "tuo", "luo", "dan", \
	"xiao", "na|ruo", "jian", "xuan", "bian", "sun", "xiang", "xian", "ping", "jian|zhen", \
	"sheng|xing", "hu", "shi|yi", "zhu|zhuo", "yue|yao|chuo", "chun", "lv", "wu", "dong", "xiao|qiao|shuo", \
	"ji", "jie", "huang", "xing", "mei", "fan", "chuan|duan", "zhuan", "pian", "feng", \
	"zhu", "hong", "qie", "hou", "qiu", "miao", "qian", "gu", "kui", "shi", \
	"ju|lou|lv", "xun|yun", "he", "tang", "yue", "chou", "gao", "fei", "na|ruo", "zheng", \
	"gou", "nie", "qian", "xiao", "cuan", "gong|gan", "pang|peng", "du", "li", "bi|pi", \
	"huo|zhuo", "chu", "shai|shi", "chi", "zhu", "cang|qiang", "long", "lan", "jian", "bu", \
	"li", "hui", "bi", "zhu|di", "cong", "yan", "peng", "sen|zan", "zuan|zhuan", "pi", \
	"biao|piao", "dou", "yu", "mie", "zhuan|tuan", "ze|zhai", "shai", "guo|gui", "yi", "hu", \
	"chan", "kou", "chuo|cou|cu", "ping", "zao", "ji", "gui", "su", "ju|lou|lv", "ce|ji", \
	"lu", "nian", "suo|sui", "cuan", "diao", "suo", "le", "duan", "liang", "xiao", \
	"bo", "mie|mi", "si|shai", "tang|dang", "liao", "dan", "dian", "fu", "jian", "min", \
	"kui", "dai", "jiao", "deng", "huang", "zhuan|sun", "lao", "zan", "xiao", "lu", \
	"shi", "zan", "qi", "pai", "qi", "pi", "gan", "ju", "lu", "lu", \
	"yan", "bo", "dang", "sai", "ke|zhua", "gou", "qian", "lian", "bao|bo|bu", "zhou", \
	"lai", "shi", "lan", "kui", "yu", "yue", "hao", "zhen|jian", "tai", "ti", \
	"mi|nie", "chou|tao", "ji|jie", "yi", "qi", "teng", "zhuan", "zhou", "fan|ban", "shu|sou", \
	"zhou", "qian", "zhuo", "teng", "lu", "lu", "jian", "tuo", "ying", "yu", \
	"lai", "long", "qie", "lian", "lan", "qian", "yue", "zhong", "ju|qu", "lian", \
	"bian", "duan", "zuan", "li", "shi|shai", "luo", "ying", "yue", "zhuo", "xu|yu", \
	"mi", "di|za", "fan", "shen", "zhe", "shen", "nv", "he", "lei", "xian", \
	"zi", "ni", "cun", "zhang", "qian", "zhai", "bi|pi", "ban", "wu", "chao|sha", \
	"kang|jing", "rou", "fen", "bi", "cui|sui", "yin", "zhe", "mi", "tai", "hu", \
	"ba", "li", "gan", "ju", "po", "mo|yu", "cu", "zhan|nian", "zhou", "chi|li", \
	"su", "diao|tiao", "li", "xi", "su", "hong", "tong", "ci|zi", "ce|se", "yue", \
	"zhou|yu", "lin", "zhuang", "bai", "lao", "fen", "er", "qu", "he", "liang", \
	"xian", "fu", "liang", "can", "jing", "li", "yue", "lu", "ju", "qi", \
	"cui|sui", "bai", "zhang", "lin", "zong", "jing|qing", "guo|hua", "hua", "shen|san", "shen|san", \
	"tang", "bian", "rou", "mian", "hou", "xu", "zong", "hu", "jian", "zan", \
	"ci", "li", "xie", "fu", "nuo", "bei", "gu", "xiu", "gao", "tang", \
	"qiu", "jia", "cao", "zhuang", "tang", "mi|mei", "shen|san", "fen", "zao", "kang", \
	"jiang", "mo", "san", "san", "nuo", "xi|chi", "liang", "jiang", "kuai", "bo", \
	"huan", "shu", "ji", "han|xian", "nuo", "tuan", "nie", "li", "zuo", "di", \
	"nie", "diao|tiao", "lan", "si|mi", "si", "jiu", "xi|ji", "gong", "zheng", "jiao|jiu", \
	"you", "ji", "cha", "zhou", "xun", "yue|yao", "hong|gong", "ou|yu", "he|ge", "wan", \
	"ren", "wen", "wen", "qiu", "na", "zi", "tou", "niu", "fou", "jie|ji", \
	"shu", "chun|zhun", "pi|bi", "zhen", "miao|sha", "hong", "zhi", "ji", "fen", "yun", \
	"ren", "dan", "jin", "su", "fang|bang", "suo", "cui|zu", "jiu", "zha|za", "ba|ha", \
	"jin", "fu", "zhi", "qi", "zi", "chou|zhou", "hong", "zha|za", "lei|lv", "xi", \
	"fu", "xie|yi", "shen", "bo|bi", "shu|zhu", "qu", "ling", "zhu", "chao|shao", "gan", \
	"yang", "fu|fei", "tuo", "jin|tian|zhen", "dai", "chu", "shi", "zhong", "xuan|xian", "zu|qu", \
	"jiong", "ban", "qu", "mo", "shu", "zui", "kuang", "jing", "ren", "hang", \
	"xie|yi", "jie|ji", "zhu", "chou", "gua|kua", "bai|mo", "jue", "kuang", "hu", "ci", \
	"huan|geng", "geng", "tao", "jie|xie", "ku", "jiao|xiao", "quan", "ai|gai", "luo|lao", "xuan|xun", \
	"bing|beng", "xian", "fu", "gei|ji", "tong|dong", "rong", "diao|tiao|dao", "yin", "lei", "xie", \
	"juan", "chu|na|nv|xu", "hai|gai", "die", "tong", "si", "jiang", "xiang", "gui|hui", "jue", \
	"zhi", "jian", "juan|xuan", "chi|zhi", "wan|wen|man", "zhen", "lv", "cheng", "qiu", "shu", \
	"bang", "tong", "shao|xiao", "huan|wan", "qin|xian", "geng|bing", "xiu|xu", "ti", "xiu|tou", "xie", \
	"hong", "xi", "fu", "ting", "rui|shuai|sui", "dui", "kun", "fu", "jing", "hu", \
	"zhi", "xian|yan", "jiong", "feng", "ji", "xu", "ren", "zong|zeng", "lin|chen|shen", "duo", \
	"li|lie", "lv|lu", "liang", "chou|tao", "quan", "shao|chao", "qi", "qi", "zhun", "qi", \
	"wan", "qing|qian", "xian", "shou", "wei|yi", "qi|qing", "tao", "wan", "gang", "wang", \
	"beng", "zhui|chuo", "cai", "guo", "zu|cui", "lun|guan", "liu", "qi|yi", "zhan", "bi", \
	"chuo|chao", "ling", "mian", "qi", "qie", "tan|tian", "zong", "hun|gun", "zou", "xi", \
	"zi", "xing", "liang", "jin", "fei", "rui", "hun|mian|min", "yu", "cong|zong", "fan", \
	"lv|lu", "xu", "ying", "shang", "qi", "xu", "xiang", "jian", "ke", "xian", \
	"ruan", "mian", "ji|qi", "duan", "zhong|chong", "di", "min|mian", "miao|mao", "yuan", "xie|ye", \
	"bao", "si", "qiu", "bian", "huan", "geng", "cong|zong", "mian", "wei", "fu", \
	"wei", "xu|shu|tou", "gou", "miao", "xie", "lian", "zong", "bian|pian", "yun|gun", "yin", \
	"ti", "gua|wo", "zhi", "wen|yun", "cheng", "chan", "dai", "xia", "yuan", "zong", \
	"xu", "sheng|ying", "wei", "geng", "seon", "ying", "jin", "yi", "zhui", "ni", \
	"bang", "hu|gu", "hu|pan", "chao|cu|zhou", "jian", "cuo|ci", "quan", "shuang", "yun|wen", "xia", \
	"shuai|sui|cui", "xi|ji", "rong", "tao", "fu", "yun", "zhen|chen", "gao", "rong|ru", "hu", \
	"zai|zeng", "teng", "xian|xuan", "su", "zhen", "cong|zong", "tao", "huang", "cai", "bi", \
	"feng", "cu", "li", "suo|su", "yan|yin", "xi", "cong|zong", "lei", "zhuan|juan", "qian", \
	"man", "zhi", "lv", "mo|mu", "piao", "lian", "mi", "xuan", "cong|zong", "ji", \
	"shan|xian|sao", "cui|sui", "fan|po", "lv", "beng", "yi", "sao|zao", "miu|miao|liao|mou|mu", "yao|you|zhou", "qiang", \
	"sheng|hun", "xian", "xi|ji", "sha", "xiu", "ran", "xuan", "sui", "qiao|jue", "ceng|zeng", \
	"zuo", "zhi", "shan", "san", "lin", "ju|yu|jue", "fan", "liao|rao", "chuo|chao", "zun", \
	"jian", "rao", "chan", "rui", "xiu", "hui", "hua", "zuan", "xi", "qiang", \
	"yun", "da", "min|sheng|ying", "hui|gui", "xi|ji", "se", "jian", "jiang", "huan", "zao|sao|qiao", \
	"cong", "xie|jie", "he|jiao|zhuo", "bi", "tan|chan|dan", "yi", "nong", "sui", "yi|shi", "sha|shai", \
	"ru|xu", "ji", "bin", "qian", "lan", "fu|pu", "xun", "zuan", "qi", "peng", \
	"li|yao", "mo", "lei", "xie", "zuan", "kuang", "you", "xu", "lei", "xian|jian", \
	"chan", "jiao", "lu", "chan", "ying", "shan|cai", "rang|xiang", "xian|jian", "zui", "zuan", \
	"luo", "li|xi", "dao|du", "lan", "lei", "lian", "si", "jiao|jiu", "ou|yu", "hong|gong", \
	"zhou", "xian|qian", "he|ge", "yue|yao", "ji", "wan", "kuang", "ji", "ren", "wei", \
	"yun", "hong", "chun|quan|tun|zhun", "pi|bi", "miao|sha", "gang", "na", "ren", "cong|zong", "lun|guan", \
	"fen", "zhi", "wen", "bang|fang", "zhu", "zhen", "niu", "shu", "xian", "gan", \
	"xie|yi", "fu", "lian", "qu|zu", "shen", "xi", "zhi", "zhong", "chao|cu|zhou", "ban", \
	"fei|fu", "chu", "chao|shao", "shi|yi", "jing", "dai", "bang", "rong", "ji|jie", "ku", \
	"rao", "die", "hang", "gui|hui", "gei|ji", "xuan|xun", "jiang", "luo|lao", "jue", "jiao|xiao", \
	"tong", "bing|geng", "shao|xiao", "juan|xuan", "tou|xiu", "xi", "rui|shuai|sui", "tao", "ji", "ti", \
	"ji", "xu", "ling", "ying", "xu", "qi|yi", "fei", "chuo|chao", "shang", "gun|hun", \
	"min|sheng|ying", "wei|yi", "mian", "shou", "beng", "chou|diao|tao", "tao", "liu", "quan", "zong|zeng", \
	"zhan", "wan", "lv|lu", "chuo|zhui", "zi", "ke", "xiang", "jian", "mian", "lan", \
	"ti", "miao", "ji|qi", "wen|yun", "hui", "si", "duo", "duan", "bian|pian", "xian", \
	"gou", "zhui", "huan", "di", "lv", "bian", "min|mian", "yuan", "jin", "fu", \
	"rong|ru", "chen|zhen", "feng", "shuai|sui|cui", "gao", "chan", "li", "yi", "jian", "bin", \
	"piao", "man", "lei", "ying", "suo|su", "miu|miao|mou", "sao|zao", "xie", "liao|rao", "shan", \
	"ceng|zeng", "jiang", "qian", "zao|sao|qiao", "huan", "he|jiao|zhuo", "zuan", "fou", "xie", "gang", \
	"fou", "kui|que", "fou", "qi", "bo", "ping", "xiang", "zhao|diao", "gang", "ying", \
	"ying", "qing", "xia", "guan", "zun", "tan", "cang", "qi", "weng", "ying", \
	"lei", "tan", "lu", "guan", "wang", "si|wang", "gang|wang", "wang", "han", "ra", \
	"luo", "fu", "mi|shen", "fa", "gu", "zhu", "ju", "mao|meng", "gu", "min", \
	"gang", "ba", "gua", "ti|kun", "juan", "fu", "shen", "yan", "zhao", "zui", \
	"gua|guai", "zhuo", "yu", "zhi", "an", "fa", "lan", "shu", "si", "pi", \
	"ma", "liu", "ba|pi", "fa", "li", "chao", "wei", "bi", "ji", "zeng", \
	"chong", "liu", "ji", "juan", "mi", "zhao", "luo", "pi", "ji", "ji", \
	"luan", "yang", "mi", "qiang", "da", "mei", "yang|xiang", "you", "you", "fen", \
	"ba", "gao", "yang", "gu", "qiang|you", "yang|zang", "mei|gao", "ling", "yi|xi", "zhu", \
	"di", "xiu", "qian|qiang", "yi", "xian|yan|yi", "rong", "qun", "qun", "qian|qiang", "huan", \
	"suo|zui", "xian|yan", "xi|yi", "yang", "kong|qiang", "xian|qian", "yu", "geng|lang", "jie", "tang", \
	"yuan", "xi", "fan", "shan", "fen", "shan", "lian", "lei|lian", "geng|lang", "nou", \
	"qiang", "chan", "hu|yu", "gong|hong", "yi", "chong", "weng", "fen", "hong", "chi", \
	"chi", "cui", "fu", "xia", "ben|pen", "yi", "la", "yi", "po|pi|bi", "ling", \
	"lu|liu", "zhi", "yu|qu", "xi", "xie", "xiang", "xi", "xi", "ke", "qiao", \
	"hui", "hui", "shu|xiao", "sha", "hong", "jiang", "zhai|di", "cui", "fei", "zhou|dao", \
	"sha", "chi", "zhu", "jian", "xuan", "chi", "pian", "zong", "wan", "hui", \
	"hou", "he|li", "hao|he", "han", "ao", "piao", "yi", "lian", "qu|hou", "ao", \
	"lin", "pen", "qiao", "ao", "fan", "yi", "hui", "xuan", "dao", "yao", \
	"lao", "uu", "kao", "mao", "zhe", "shi|qi", "gou", "gou", "gou", "die", \
	"die", "er|neng", "shua", "ruan|nuo", "er|nai", "nai|neng", "zhuan|duan", "lei", "ting", "zi", \
	"geng", "chao", "hao|mao", "yun", "pa|ba", "pi", "yi|chi", "si", "qu|chu", "jia", \
	"ju", "huo", "chu", "lao", "lun", "jie|ji", "tang", "ou", "lou", "nou", \
	"jiang", "pang", "ze|zha", "lou", "ji", "lao", "huo", "you", "mo", "huai", \
	"er|reng", "yi", "ding", "xie|ye", "da|zhe", "song", "qin", "ying|yun", "chi", "dan", \
	"dan", "hong", "geng", "zhi", "uu", "nie|she|ye|zhe", "dan", "zhen", "che", "ling", \
	"zheng", "you", "wa|tui", "liao|liu", "long", "zhi", "ning", "tiao", "nv|er", "ya", \
	"zhe|tie", "guo", "sei", "lian", "hao", "sheng", "lie", "pin", "jing", "ju", \
	"bi", "di|zhi", "guo", "wen", "xu", "ping", "cong", "ding", "uu", "ting", \
	"ju", "cong", "kui", "lian", "kui", "cong", "lian", "weng", "kui", "lian", \
	"lian", "cong", "ao|you", "sheng", "song", "ting", "kui", "nie|she|ye|zhe", "te|zhi", "dan", \
	"ning", "qie", "jian|ni", "ting", "ting", "long", "yu", "nie|pianpang", "zhao", "si", \
	"su", "si|yi", "su", "si|ti", "zhao", "zhao", "rou|ru", "yi", "lei|le", "ji", \
	"qiu", "ken", "cao", "ge|qi", "di|bo", "huan", "huang", "chi", "ren", "xiao", \
	"ru", "zhou", "yuan", "du", "gang", "rong|chen", "gan", "cha", "wo", "chang", \
	"gu", "shi|zhi", "han|qin", "fu|lu", "fei", "fen|ban", "pei", "feng|pang", "jian|xian", "fang", \
	"chun|tun|zhun|zhuo", "you", "na|nv", "ang|gang|hang", "ken", "ran", "gong", "yu|yo", "wen", "yao", \
	"qi", "bi|pi", "qian|xu", "xi|bi", "xi", "fei|pie", "ken", "jing", "tai", "shen", \
	"zhong", "chan|zhang", "xi|xian|xie", "chen|shen", "wei", "zhou", "die", "da|dan|tan", "fei|bi", "ba", \
	"bo", "chun|qu|xu", "tian", "bei", "gu|gua|hu", "tai", "fei|zi", "fei|ku", "shi|zhi", "ni", \
	"ping|peng", "ci|zi", "fu|zhou", "pang|pan", "zhen|zhun", "xian", "zuo", "pei", "jia", "qing|sheng|xing", \
	"chi|di|zhi", "bao|pao", "mu", "qu", "hu", "ke", "chi", "yin", "xu", "yang", \
	"long", "dong", "ka", "lu", "jing|keng", "nu", "yan", "pang", "kua", "yi", \
	"guang", "gai|hai", "ge|ga", "dong", "zhi|chi", "jiao|xiao", "xiong", "xiong", "er", "an|e", \
	"heng", "pian", "neng|nai", "zi", "gui|kui", "cheng|zheng", "tiao", "zhi", "cui", "mei", \
	"xi|xian|xie", "cui", "xie|xian|xi", "mai|mo", "mai|mo", "ji", "xie|xian|xi", "nin", "kuai", "sa", \
	"zang", "qi", "nao", "mi", "nong", "ji|luan", "wan|wen", "bo", "wen", "huan|wan", \
	"xiu", "jiao|jue", "jing|keng", "rou|you", "heng", "cuo|qie", "luan|lie", "shan|chan", "ting", "mei", \
	"chun", "shen", "jia|qian", "te|de", "zui|juan", "ji|cu", "xiu|you|xiao", "xin|chi", "tui|tuo", "pao", \
	"cheng", "nei|tui", "pu|fu", "dou", "tui|tuo", "niao", "nao", "pi", "gu", "luo", \
	"lei|li", "lian", "zhang|chang", "cui|sui", "jie", "liang|lang", "shui", "bi|pai|pi", "biao", "lun", \
	"pian", "guo|lei", "juan|quan|kui", "chui|hou", "dan", "tian", "nei", "jing", "nai", "la|xi", \
	"ye", "yan|a", "ren|dian", "shen", "zhui|chuo", "fu", "fu", "ju", "fei", "kong|qiang", \
	"wan", "dong", "bi|pai|pi", "guo|huo", "zong", "ding", "wo", "mei", "ruan|nen|ni", "zhuan|dun", \
	"chi", "cou", "luo", "ou", "di", "an", "xing", "nao", "shu|yu", "shuan", \
	"nan", "yun", "zhong", "rou", "e", "sai", "dun|tu", "yao", "jian|qian", "wei", \
	"jiao|jue", "yu", "jia", "duan", "bi", "chang", "fu", "xian", "ni", "mian", \
	"wa", "teng", "tui", "bang|pang", "xian|qian", "lv", "wa", "shou", "tang", "su", \
	"zhui", "ge", "yi", "bo|lie|po", "liao", "ji", "pi", "xie", "gao", "lv", \
	"bin", "ou", "chang", "lu|biao", "guo|huo", "pang", "chuai", "biao|piao", "jiang", "lu|fu", \
	"tang", "mo", "xi", "zhuan|chuan", "lu", "hao|jiao", "ying", "lv|lou", "zhi", "xue", \
	"cen", "lian|lin", "tong|chuang", "peng", "ni", "chuai|zha|zhai", "liao", "cui", "gui|kui", "xiao", \
	"teng|tun", "pan|fan", "zhi", "jiao", "shan", "wu|hu", "cui", "run|yin", "xiang", "sui|wei", \
	"fen", "ying", "shan|dan", "zhua", "dan", "kuai", "nong", "tun", "lian", "bi|bei", \
	"yong", "ju|jue", "chu", "yi", "juan", "ge|la", "lian", "sao", "tun", "gu", \
	"qi", "cui", "bin", "xun", "ru|nao", "wo|yue", "zang", "xian", "biao", "xing", \
	"kun", "la|lie", "yan", "lu", "huo", "za", "luo", "qu", "zang", "luan", \
	"luan|ni", "za|zan", "chen", "xian|qian", "wo", "guang|jiong", "zang|cang", "lin", "jiong|guang", "zi", \
	"jiao", "nie", "chou|xiu", "ji", "gao|gu|hao", "chou|xiu", "bian|mian", "nie", "die|zhi", "zhi|zhui", \
	"ge", "jian", "zhi|die", "zhi|jin", "xiu", "tai", "zhen", "jiu", "xian", "kui|yong|yu", \
	"cha", "yao", "yu", "chong|chuang|zhong", "que|xi", "que|tuo|xi", "jiu", "yu", "yu", "xin|xing", \
	"ju", "jiu", "xin|wen", "gua|she", "she|shi", "she", "jiu", "shi", "ran|tan", "shu|yu", \
	"shi", "tan|tian", "tan", "pu", "hu|pu", "guan", "hua|qi", "tian", "chuan", "shun", \
	"xia", "wu", "zhou", "dao", "chuan|xiang", "shan", "yi", "fan", "pa", "tai", \
	"fan", "ban", "chuan|fan", "hang", "fang", "ban|pan|bo", "bi", "lu", "zhong", "jian", \
	"cang", "ling", "zhou|zhu", "ze", "duo|tuo", "bo", "xian", "ge", "chuan", "xia", \
	"lu", "qiong|hong", "feng|pang", "xi", "kua", "fu", "zao", "feng", "li", "shao", \
	"yu", "lang", "ting", "uu", "wei", "bo", "meng", "nian|qian", "ju|keo", "huang", \
	"shou", "zong|ke", "bian", "mu|mo", "die", "dou", "bang", "cha", "yi", "sou", \
	"cang", "cao", "lou", "dai", "xue", "yao|tiao", "chong|tong|zhuang", "deng", "dang", "qiang", \
	"lu", "yi", "ji", "jian", "huo|wo", "meng", "qi", "lu", "lu", "chan", \
	"shuang", "gen|hen", "liang", "jian", "jian", "se|shai", "yan", "bo|fu|pei", "ping", "yan", \
	"yan", "cao", "cao|ao", "yi", "le|ji", "ding|ting", "jiao|qiu", "ai|yi", "nai|reng", "tiao", \
	"jiao", "jie", "peng", "wan", "yi", "cha", "mian", "mi|mie", "gan", "qian", \
	"xu|yu", "xu|yu", "di|que|shao|xiao", "xiong", "du", "xia|hu", "qi", "huang|mang|wang", "zi", "hui|hu", \
	"sui", "zhi", "xiang", "pi|bi", "fu", "tun|chun", "wei", "wu", "zhi", "qi", \
	"shan|wei", "wen", "qian", "ren", "fu|fou", "kou", "jie|gai", "hu|lu", "xu|zhu", "ji", \
	"qin|yin", "chi|qi", "yuan|yan", "fen", "ba|pa", "rui|ruo", "xin", "ji", "hua", "hua", \
	"fang", "wu|hu", "jue", "ji", "zhi", "yun", "qin", "ao", "zou|chu", "mao", \
	"ya", "fu|fei", "reng", "hang", "cong", "yin|chan", "you", "bian", "yi", "qie", \
	"wei", "li", "pi", "e", "wan|xian", "chang", "cang", "zhu", "su", "di|ti", \
	"yu|yuan|yun", "ran", "lian|ling", "tai", "tiao|shao", "di", "miao", "qing", "li|ji", "yong", \
	"he|ke", "mu", "bei", "bao|biao|pao", "gou", "min", "yi", "yi", "ju|qu", "pi|pie", \
	"ruo|re", "gu|hu|ku", "zhu|ning", "ni", "pa|bo", "bing", "chan|shan|tian", "xiu", "yao", "xian", \
	"ben", "hong", "yang|ying", "zha|zuo", "dong", "ju|cha", "die", "nie", "gan", "hu", \
	"peng|ping", "mei", "fu|pu", "sheng|rui", "gua|gu", "bi|bie", "wei", "fu|bo", "zhu|zhuo", "mao", \
	"fan", "qie|jia", "mao", "mao", "ba|fei|pei", "ci|zi", "mo", "zi", "zhi", "chi", \
	"ji", "jing", "long", "cong", "niao", "uu", "xue", "ying", "qiong", "ge|luo", \
	"ming", "li", "rong", "yin", "gen|jian", "qian|xi", "chai|zhi", "chen", "wei|yu", "xiu|hao", \
	"zi", "lie", "wu", "duo|ji", "gui", "ci", "chong|jian", "ci", "hou|gou", "guang", \
	"huang|mang", "cha|chi", "jiao|qiao|xiao", "niao|jiao", "fu", "yu", "zhu", "zi|ci", "jiang", "hui", \
	"yin", "cha", "fa|pei", "rong", "ru", "chong", "mang|mu", "tong", "zhong", "qian", \
	"zhu", "xun", "huan", "fu", "chuo|quan", "gai", "ta|da", "jing", "xing", "chuan", \
	"cao|zao", "jing", "er", "an", "qiao", "chi|qi", "ren", "jian", "yi|ti", "huang|kang", \
	"peng|ping", "li", "jin|qian", "lao|cha", "shu", "zhuang", "da", "jia", "rao|yao", "bi", \
	"ce", "qiao", "hui", "ji|qi", "dang|tang", "yu", "rong", "hun|xun", "ying|xing", "luo", \
	"ying", "qian|xun", "jin", "sun", "yin", "mai", "hong", "zhou", "lue|shuo|yao|yue", "du", \
	"wei", "li", "dou", "fu", "ren", "yin", "he", "bi", "bu|pu", "yun", \
	"di", "cha|shu|tu|ye", "sui|wei", "sui", "cheng", "chen|nong", "wu", "bie", "xi", "geng", \
	"li", "fu|pu", "zhu", "mo", "chi|li", "zhuang", "ji|zuo", "tuo", "qiu", "sha|suo", \
	"suo", "chen", "feng|peng", "ju", "mei", "meng|qing|xi", "xing", "jing|ying", "che", "shen|xin", \
	"jun", "yan", "ting", "di|diao|you", "cuo", "guan|wan", "han", "xiu|you", "cuo", "jia", \
	"wang", "you|su", "niu|rou", "xiao|shao", "wan|xian", "liang|lang", "piao|fu", "e", "mo|wu", "mian|wan|wen", \
	"jie", "nan", "mu", "kan", "lai", "lian", "shi", "wo", "tu", "xian|lian", \
	"huo", "you", "ying", "ying", "neus", "chun", "mang", "mang", "ci", "wan|yun|yu", \
	"jing", "di", "qu", "dong", "guan|jian", "zou|cuan|chu", "gu", "la", "lu", "ju", \
	"wei", "jun", "ren|nie", "kun", "ge|he", "pu", "zi|zai", "gao", "guo", "fu", \
	"lun", "chang", "chou", "song", "chui", "zhan", "men", "cai", "ba", "li", \
	"tu", "bo", "han", "bao", "qin", "juan", "si|xi", "qin", "di", "sha|jie", \
	"bei|bo|pu", "dang", "jin", "zhao|qiao", "zhi|tai|chi", "geng", "hua|kua", "gu", "ling", "fei", \
	"jin|qin", "an|yan", "wang", "beng", "zhou", "yan|yu", "zu|ju", "jian", "lin", "tan", \
	"jiao|shu", "tian", "dao", "hu", "ji|qi", "he", "cui", "tao", "chun", "ba|bei|bi|pi", \
	"chang", "huan", "fei|fu", "lai", "qi", "meng|ming", "ping", "wei", "dan|wei", "sha", \
	"huan|zhui", "yan|juan", "yi", "shao|tiao", "qi|ji|ci", "guan|wan", "ce", "nai", "zhen", "tuo|ze", \
	"jiu", "tie", "luo", "bi", "yi", "meng", "be", "pao", "ding", "ying", \
	"ying", "ying", "xiao", "sa", "jiao|qiu", "ke", "xiang", "wan", "yu|ju", "yu", \
	"fu|bei", "lian", "xuan", "xuan", "nan", "ce", "wo", "chun", "xiao|shao", "yu", \
	"bian|pian", "mao|mu", "an", "e", "luo|la|lao", "ying", "huo|kuo", "kuo", "jiang", "mian|wan", \
	"ze|zuo", "zuo", "zu|ju", "bao", "rou", "xi", "she|ye", "an|yan", "qu", "jian", \
	"fu", "lv", "jing|jian", "pen|fen", "feng", "hong", "hong", "hou", "yan|xing", "tu", \
	"zhu|zhuo|zhe", "zi", "xiang", "shen|ren", "ge", "qia", "jing|qing", "mi", "huang", "shan|shen", \
	"bei|pu", "gai", "dong|zhong", "zhou", "qian|jian", "wei", "bo", "wei", "pa", "ji", \
	"hu", "zang", "jia|xia", "duan", "yao", "jun|sui", "chuang|cong", "quan", "wei", "zhen|qian", \
	"kui", "ding|ting", "hun|xun", "xi", "shi", "qi", "lan", "zong", "yao", "yuan", \
	"mei", "yun", "shu", "di", "zhuan", "guan", "ran", "xue", "chan", "kai", \
	"kui|kuai", "uu", "jiang", "ju|liu|lou|lv", "wei|hua", "pai", "yong|you", "hui|sou", "yin", "shi", \
	"chun", "shi", "yun", "zhen", "lang", "ru|na", "meng|weng", "li", "que", "suan", \
	"yuan|huan", "li", "ju", "xi", "bang|pang", "chu", "xu|shu", "tu", "liu", "huo|wo", \
	"dian", "qian", "zu|ju", "po", "cuo", "yuan", "chu", "yu", "kuai", "pan", \
	"pu", "bo|pu", "na", "shuo", "xi", "fen", "yun", "zheng", "jian", "ji", \
	"ruo", "cang", "en", "mi", "gao|hao", "sun", "qin|zhen", "ming|mi", "sou", "xu", \
	"liu", "xi", "gu", "lang", "rong", "weng", "gai|ge", "cuo", "shi", "tang", \
	"luo", "ru", "sui|suo", "xuan", "bei", "yao|zhuo", "gui", "bi", "zong", "gun", \
	"zuo", "tiao", "ce", "pei", "la|lan", "uu", "ji", "li", "shen", "lang", \
	"yu", "ling", "ying", "ma|mo", "tiao|di|you|diao", "xiu|tiao", "mao", "tong", "zhu|chu", "peng", \
	"an", "lian", "cong|zong", "xi", "ping", "xu|ou|qiu|fu", "jin", "tuan|chun", "jie", "wei", \
	"tui", "cao", "yu", "yi", "zi|ju", "liao|lu", "bi", "lu", "xu", "bu", \
	"zhang", "lei", "qiang|jiang", "man", "yan", "ling", "ji|xi", "piao|biao", "gun", "han", \
	"di", "su", "lu|cu", "she", "shang", "di", "mie", "hun|xun", "man|wan", "bo", \
	"dai|di", "cuo|cu", "zhe", "san|shen", "xuan", "wei|yu", "hu", "ao", "mi", "lou|lv", \
	"chuo|cou|cu", "zhong", "ca|cai|sa", "po|bo", "jiang", "mi", "cong|chuang", "niao", "hui", "juan|jun", \
	"yin", "jian|shan", "nian|yan", "shu", "yin", "guo", "chen", "hu", "sha", "kou", \
	"qian", "ma", "zang|cang", "ze", "qiang|se", "dou", "lian|xian", "lin", "kou", "ai", \
	"bi|bie|pie", "li", "wei", "ji", "xun|qian", "sheng", "fan|bo", "meng", "ou", "chan", \
	"dian", "tan|xun", "jiao|qiao", "juan|rui", "juan|rui", "lei", "yu", "jiao|qiao", "zhu|chu", "hua|kua", \
	"jian", "mai", "yun", "bao", "you", "qu", "lu", "rao|yao", "hui", "e", \
	"ti", "fei", "jue", "jue|zhuo|zui", "fa|fei", "ru", "fen|fei", "kui|kuai", "shun", "rui", \
	"ya", "xu", "fu", "jue", "dang|tang", "wu", "dong", "si", "xiao", "xi", \
	"long|sa", "wen|yun", "shao", "ji|qi|qin", "jian", "wen|yun", "sun", "ling", "yu", "xia", \
	"weng|yong", "ji|qie", "hong", "si", "nong", "lei", "xuan", "yun|wen", "yu", "xi|xiao", \
	"hao", "bao|bo", "hao", "ai", "wei", "hui", "hui", "ji", "zi|ci", "xiang", \
	"luan|wan", "mie", "yi", "leng", "jiang", "can", "shen", "qiang|se", "lian", "ke", \
	"yuan", "da", "zhi|ti", "tang", "xue", "bai|bi|bo", "zhan", "sun", "lian|xian", "fan", \
	"ding", "xie", "gu", "xie", "shu|zhu", "jian", "hao|kao", "hong", "sa", "xin", \
	"xun", "yao", "bai", "cou|shu|sou", "shu", "xun", "dui", "pin", "yuan|wei", "ning", \
	"zhou|chou", "mai|wo", "ru", "piao", "tai", "qi|ji|ci", "zao", "chen", "zhen", "er", \
	"ni", "ying", "gao", "cong", "hao|xiao", "qi", "fa", "jian", "yu|xu", "kui", \
	"ji|jie", "bian", "di|diao", "mi", "lan", "jin", "cang|zang", "miao|mo", "qiong", "qie", \
	"xian", "uu", "ou", "xian|qian", "su", "lv", "yi", "xu", "xie", "li", \
	"yi", "la", "lei", "jiao", "di", "zhi", "bei", "teng", "yao|yue|shuo", "mo", \
	"huan", "biao|pao", "fan", "cou|shu|sou", "tan", "tui", "qiong", "qiao", "wei", "liu", \
	"hui", "ou", "gao|kao", "yun|wen", "bao", "li", "zhu|shu", "zhu|chu", "ai", "lin", \
	"zao", "xuan", "qin", "lai", "he|huo", "ze|tuo", "e|wu", "rui", "rui", "ji|qi", \
	"heng", "lu", "su", "tui", "mang", "yun|wen", "ping|pin", "yu", "xun", "ji", \
	"jiong", "xuan", "mo", "qiu", "su", "jiong", "feng", "bo|nie", "bi|bo", "xiang|rang", \
	"yi", "xian", "yu", "ju", "lian|xian", "lian|xian", "yin", "qiang", "ying", "long", \
	"tou", "hua", "yue", "ling", "ju|qu", "yao", "fan", "mei", "lan|han", "kui|hui", \
	"lan", "ji", "dang|tang", "man", "lei", "lei", "hui|hua", "feng|song", "zhi", "wei", \
	"kui", "zhan", "huai", "li", "ji", "mi", "lei", "huai", "luo", "ji", \
	"kui", "lu", "jian", "sal", "teng", "lei", "quan", "xiao", "yi", "luan", \
	"men", "bie", "hu|pianpang", "hu", "lu", "nue|nve", "bi|lv", "si|ti|zhi", "xiao", "qian", \
	"chu|ju", "hu", "xu", "cuo", "fu", "xu", "xu", "lu", "hu", "yu", \
	"hao", "jiao|hao", "ju", "guo", "bao", "yan", "zhan", "zhan", "kui", "bin", \
	"se|xi", "shu", "chong|hui", "qiu", "dao|diao", "ji", "qiu", "ding|cheng", "shi", "uu", \
	"jue", "zhe", "she|ye", "yu", "gan|han", "zi", "gong|hong|jiang", "hui", "meng", "ge", \
	"sui", "xia|ha", "chai", "shi", "yi", "ma", "xiang", "fang|bang", "e", "ba", \
	"chi", "qian", "wen", "wen", "rui", "beng|bang", "pi", "yue", "yue", "jun", \
	"qi", "tong", "yin", "zhi|qi", "can|tian", "yuan|wan", "que|jue", "you|hui", "qin|qian", "qi", \
	"zhong", "ya", "ci|hao", "mu", "wang", "fen", "fen", "hang", "gong|zhong", "zao|zhao", \
	"fu", "ran", "jie", "fu", "chi", "dou", "bao|pao", "xian", "ni", "dai|te", \
	"qiu", "you|zhu", "zha", "ping", "chi|di", "niu|you", "he|ke", "han", "ju", "li", \
	"fu", "ran|tian", "zha", "qu|gou", "pi", "pi|bo", "xian", "zhu", "diao", "bie", \
	"bing", "gu", "zhan", "ju|qu", "she|yi", "tie", "ling", "gu", "dan", "gu", \
	"ying", "li", "cheng", "qu", "mao|mou", "ge|luo", "ci", "hui", "hui", "mang|bang", \
	"fu", "yang", "jue|wa", "lie", "zhu", "yi", "xian", "kuo|she", "jiao", "li", \
	"xu|yi", "ping", "jie|qie", "ge|ha", "she", "yi", "wang", "mo", "gong|qiong", "qie|ni", \
	"gui", "qiong", "zhi", "man", "lao|e", "zhe", "jia", "nao", "si", "qi", \
	"xing", "jie", "qiu", "xiao|shao", "yong", "jia", "tui|yue", "che", "bei", "e|yi", \
	"han", "shu", "xuan", "feng", "shen", "zhen|shen", "pu|fu", "xian", "zhe", "wu", \
	"fu", "li", "liang|lang", "bi", "chu|yu", "xuan|yuan", "you", "jie", "dan", "dan|yan", \
	"dian|ting", "dian", "tui|yue", "hui", "wo", "zhi", "song", "bei|fei|pei", "ju", "mi", \
	"qi", "qi", "yu", "jun", "la|zha", "meng", "qiang", "xi|si", "xi", "lun", \
	"li", "die", "diao|tiao", "tao", "kun", "han", "han", "guo|yu", "bang", "fei", \
	"miao|pi", "wei", "dun|tun", "xi|yi", "yuan|yun", "suo", "juan|quan", "qian", "rui|wei", "ni", \
	"jing|qing", "tong|wei", "liang", "guo|luo", "wan", "dong", "e", "ban", "di|zhuo", "wang", \
	"can", "mi", "ying", "guo", "chan", "uu", "la", "ke", "ji|jie", "he|xie", \
	"ting", "mao", "xu|xie", "mian", "yu", "jie", "li|long|shi", "xuan", "huang", "yan", \
	"bian|pian", "rou|nao", "wei", "fu", "yuan", "mei", "wei", "fu", "ruan", "xie", \
	"you", "qiu|you", "mao|wu", "xia|ha", "ying", "shi", "zhong|chong", "tang", "zhu", "zong", \
	"ti|chi", "fu", "yuan", "kui", "meng", "la", "du|dai", "hu", "qiu", "die|tie", \
	"li|xi", "wo|luo", "ao|yun", "yu|qu", "nan", "lou", "chun", "rong", "ying", "jiang", \
	"tui|ban", "lang", "bang|pang", "si", "ci|xi", "ci", "xi|qi", "yuan", "weng", "lian", \
	"sou", "ban|pan", "rong", "rong", "ji", "wu", "xiu", "han", "qin", "yi|si", \
	"bi|pi", "hua", "tang", "yi", "du", "neng|nai", "xia|he", "hu", "gui|hui", "ma", \
	"ming", "yi", "wen", "ying", "teng|te", "zhong", "cang", "so", "qi", "man", \
	"tiao", "shang", "shi|zhe", "cao", "chi", "dai|di", "ao", "lu", "wei", "zhi|die", \
	"tang", "chen", "piao", "ju|qu", "pi", "yu", "jian|chan", "luo", "lou", "qin", \
	"zhong", "yin", "jiang", "shuai", "wen", "xiao", "man|wan", "zhe", "zhe", "ma|mo", \
	"ma", "guo|yu", "liao|liu", "mao|meng", "xi", "cong", "li", "man", "xiao", "chang", \
	"zhang", "mang|meng", "xiang", "mo", "zui", "si", "qiu", "te", "zhi", "peng", \
	"peng", "qiao|jiao", "qu", "bie", "liao", "fan|pan", "gui", "xi", "ji|qi", "zhuan", \
	"huang", "ben|fei", "lao|liao", "jue", "jue", "hui", "yin|xun", "chan|ti", "jiao", "shan", \
	"rao|nao", "xiao", "wu|mou", "chong|zhong", "xun", "si", "chu", "cheng", "dang", "li", \
	"xie", "shan|dan", "ji|yi", "jing", "da", "chan", "qi|ji", "ci|ji", "xiang", "she", \
	"guo|luo", "kem|qin", "ying", "chai", "li", "zei", "xuan", "lian", "shu|zhu", "ze", \
	"xie", "mang", "xie", "qi", "rong", "jian", "meng", "hao", "ru", "huo|yue", \
	"zhuo", "jie", "bin", "he", "mie", "fan", "lei", "jie", "la", "mian|min", \
	"li|luo", "chun", "li", "qiu", "nie", "lu", "du", "xiao", "chu|zhu", "long", \
	"li", "long", "pang", "ye", "pi", "rang|shang|nang", "gu|ye", "juan", "ying", "shu", \
	"xi", "can", "qu", "huan|quan", "du", "can", "man", "jue|qu", "jie", "shu|zhu", \
	"zhuo", "xue|xie", "huang", "nv", "fou|pei", "nv", "xin", "zhong", "mai", "er", \
	"ka|kai", "mie", "xi", "xing|hang|heng", "yan", "kan", "yuan", "qu", "ling", "xuan", \
	"shai|shu", "xian", "tong|dong", "long|xiang", "jie", "xian|yu", "ya|yu", "hu", "wei", "dao", \
	"chong", "wei", "dao", "zhun", "heng", "qu", "yi", "pianpang|yi", "bu", "gan", \
	"yu", "biao", "cha", "yi", "shan", "chen", "fu", "gun", "fen|pen", "shuai|cui", \
	"jie", "na", "zhong", "dan", "yi", "zhong", "zhong", "jie", "zhi|ti", "xie", \
	"ran", "zhi", "ren", "qin", "jin|qin", "jun", "yuan", "mei|yi", "chai", "ao", \
	"niao", "yi|hui", "ran", "jia", "tuo", "ling", "dai", "bao|pao", "bao|pao", "yao", \
	"zuo", "bi", "shao", "tan|zhan", "ju|jie", "ke|he", "xue", "xiu", "zhen", "tuo|yi", \
	"pa", "bo|fu", "di", "mo|wa", "fu", "gun", "zhi", "zhi", "ran", "fan|pan", \
	"yi", "mao|mou", "tuo", "na|jue", "gou", "xuan", "zhe|chan", "qu", "bei|bi|pi", "yu", \
	"xi", "mi|ni", "bo", "uu", "fu", "chi|nuo", "chi|qi", "ku", "ren", "jiang", \
	"qia|jia|jie", "zun|jian", "mo|bo", "jie", "er", "ge|luo", "ru", "zhu", "gui", "yin", \
	"cai", "lie", "ka", "xing", "zhuang", "dang", "sed", "kun", "ken", "niao", \
	"shu", "jia|xie", "kun", "cheng", "li", "juan", "shen", "bao|pou", "jie|ge", "yi", \
	"yu", "zhen", "liu", "qiu", "qun", "ji", "yi", "bu", "zhuang", "shui", \
	"sha", "qun", "li", "lian", "chan|lian", "ku", "jian", "bao|pou|xiu", "chan|tan", "bi|pi", \
	"kun", "tao", "yuan", "ling", "chi", "chang", "chou|dao", "duo", "biao", "liang", \
	"shang|chang", "fei|pei", "pei|fei", "fei", "yuan|gun", "luo", "guo", "an|yan", "du", "xi|ti", \
	"zhi", "ju", "yi|qi", "qi|ji", "guo", "gua", "ken", "qi", "ti", "ti|shi", \
	"fu", "chong|zhong", "xie", "bian|pian", "die", "kun", "duan|tuan", "xiu|you", "xiu|you", "he", \
	"yuan", "bao|pou", "bao", "fu", "tou|yu", "tuan", "yan", "yi|hui", "bei", "chu|zhu", \
	"lv", "pao", "dan", "yun|wen", "ta", "gou", "da", "huai", "rong", "yuan", \
	"nu|ru", "nai", "jiong", "suo|cha", "ban|pan", "tui|tun", "chi", "sang", "niao", "ying", \
	"jie", "qian", "huai", "ku", "lian", "lan", "li", "die|xi|zhe", "shi", "lv", \
	"nie|yi", "die", "xie", "xian", "wei", "biao", "cao", "ji", "qiang", "sen|shan", \
	"bao|pou", "xiang", "bi", "pu|fu", "jian", "zhuan|juan", "jian", "cuo|cui", "ji", "chan|dan", \
	"za", "fan|bo", "bo|fei", "xiang", "xun|xin", "bie", "rao", "man", "lan", "ao", \
	"ze|yi", "hui|gui", "cao", "sui", "nong", "chan|dan", "lian|chan", "bi", "jin", "dang", \
	"shu|du", "tan|zhan", "bi", "lan", "pu|fu", "ru", "zhi", "tae", "shu|du", "wa", \
	"shi", "bai|bei", "xie", "bo", "chen", "lai", "long", "xi", "shan|xian", "lan", \
	"zhe", "dai", "ju", "zan|cuan", "shi", "jian", "pan", "yi", "lan", "ya", \
	"xi", "xi|ya", "yao", "ban|feng", "qin|tan", "fu", "fiao", "fu", "ba|po", "he", \
	"ji", "ji", "jian|xian", "guan", "bian", "yan", "gui|xu", "jue|jiao", "pian", "mao", \
	"mi", "mi", "pie|mie", "shi", "si", "chan|dan", "zhen", "jue|jiao", "mi", "tiao", \
	"lian", "yao", "zhi", "jun", "xi", "shan", "wei", "xi", "tian", "yu", \
	"lan", "e", "du", "qin|qing", "pang", "ji", "ming", "ying", "gou", "qu", \
	"zhan", "jin", "guan", "deng", "jian|bian", "luan|luo", "qu", "jian", "wei", "jue|jiao", \
	"qu", "luo", "lan", "shen", "ji|di", "guan", "jian|xian", "guan", "yan", "gui|xu", \
	"mi", "shi", "chan|dan|ji", "lan", "jue|jiao", "ji", "xi", "di|ji", "tian", "yu", \
	"gou", "jin", "qu", "jiao|jue", "qiu", "jin", "cu|chu", "gui|jue|kui", "zhi", "chao", \
	"ji", "gu", "dan", "zi|zui", "di|zhi", "shang", "xie|hua", "quan", "ge", "shi", \
	"jie|xie", "gui", "gong", "chu", "jie|xie", "hun", "qiu", "xing", "su", "ni", \
	"ji|qi", "lu", "zhi", "da|zha", "bi", "xing", "hu", "shang", "gong", "zhi", \
	"hu|xue", "chu", "xi|wei", "yi", "li|lu", "jue", "xi|wei", "yan", "xi|wei", "yan|yin", \
	"yan", "ding", "fu", "kao|qiu", "qiu", "jiao", "heng|hong|jun", "ji", "fan", "xun", \
	"diao", "hong", "cha|chai", "tao", "xu", "jie|ji", "yi|dan", "ren", "xun", "yin", \
	"shan", "qi", "tuo", "ji", "xun", "yin", "e", "fen|bin", "ya", "yao", \
	"song", "shen", "jin|yin", "xin|xi", "jue", "xiao|na", "ne", "chen", "you", "zhi", \
	"xiong", "fang", "xin", "chao|miao", "she", "yan", "sa", "zhun", "xu|hu", "yi", \
	"yi", "su", "chi", "he", "shen", "he", "xu", "zhen", "zhu", "zheng", \
	"gou", "zi", "zi", "zhan|dian|che", "gu", "fu", "jian", "die", "ling", "di|ti", \
	"yang", "li", "nu|nao|na", "pan", "zhou", "gan", "yi", "ju", "yao", "zha", \
	"tuo|duo", "dai|yi", "qu", "zhao", "ping", "bi", "xiong", "chu|qu", "ba|bo", "da", \
	"zu", "tao", "zhu", "ci", "zhe", "yong", "xu", "xun", "yi", "huang", \
	"he|ge", "shi", "cha|qie", "xiao", "shi", "hen", "cha|du", "gou|hou", "gui", "quan", \
	"hui", "jie", "hua", "gai", "xiang|yang", "wei", "shen", "zhou|chou", "dong|tong", "mi", \
	"dan|zhan", "ming", "luo|e|lue", "hui", "yan", "xiong", "gua", "er|chi", "bing", "tiao|diao", \
	"chi|yi", "lei", "zhu", "kuang", "kua|qu", "wu", "yu", "teng", "ji", "zhi", \
	"ren", "cu", "lang", "e", "kuang", "xi|yi", "shi", "ting", "dan", "bei", \
	"chan", "you", "keng", "qiao", "qin", "shua", "an", "yu", "xiao", "cheng", \
	"jie", "xian", "wu", "wu", "gao", "song", "bu", "hui", "jing", "shuo|shui|yue", \
	"zhen", "shuo|shui|yue", "du", "hua", "chang", "shui|shei", "jie", "ke", "jue|qu", "cong", \
	"xiao", "sui", "wang", "xian", "fei", "lai|chi", "ta", "yi", "ni|na", "yin", \
	"diao|tiao", "bei|pi", "zhuo", "chan", "chen", "zhun", "ji", "qi|ji", "tan", "zhui", \
	"wei", "ju", "qing", "dong", "zheng", "cuo|zuo|ze|zha", "zou|zhou", "qian", "zhuo", "liang", \
	"jian", "chu|ji", "hao|xia", "lun", "nie|shen", "biao", "hua", "bian|pian", "yu", "die|xie", \
	"xu", "pian", "shi|di", "xuan", "shi", "hun", "gua|hua", "e", "zhong", "di|ti", \
	"xie", "fu", "pu", "ting", "jian|lan", "qi", "tou|yu", "zi", "zhuan", "xi|shai", \
	"hui", "yin", "an|tou", "gan|xian", "nan", "chen", "feng", "chu|zhu", "yang", "yan", \
	"huang", "xuan", "ge", "nuo", "qi|xu", "mou", "ye", "wei", "xing", "teng", \
	"zhou|chou", "shan", "jian", "pao|po", "kui|dui", "huang", "huo", "ge", "hong|ying", "mi|mei", \
	"sou|xiao", "mi", "xi|xia", "qiang", "chen|zhen", "xue", "ti|si", "su", "bang", "chi", \
	"qian|zhan", "shi|xi|yi", "jiang", "quan|yuan", "xie", "xiao|he", "tao", "yao", "yao", "zhi", \
	"xu|yu", "biao|piao", "cong", "qing", "li", "mo", "mo", "shang", "ze|zhe", "miu", \
	"jian", "ze", "zu|jie", "lian", "lou|lv", "can|zao", "ou|xu", "gun", "xi|che", "shu|zhuo", \
	"ao", "ao", "jin", "zhe", "yi|chi", "xiao|hu", "jiang", "man", "chao|zhao", "han|xian", \
	"hua|wa", "chan|dan", "xu", "zeng", "se", "xi", "zha", "dui", "zheng", "xiao|nao", \
	"lan", "e", "ying", "jue", "ji", "zun", "jiao|qiao", "bo", "hui", "quan|zhuan", \
	"mo|wu", "jian|zen", "zha", "shi|zhi", "qiao", "tan", "zen", "pu", "sheng", "xuan", \
	"zao", "tan", "dang", "sui", "xian", "ji", "jiao", "jing", "lian|zhan", "nang|nou", \
	"yi", "ai", "zhan", "pi", "hui", "hui|hua", "yi", "yi", "shan", "rang", \
	"nou", "qian", "dui", "ta", "hu", "chou|zhou", "hao", "yi|ai", "ying", "jian|kan", \
	"yu", "jian", "hui", "du|dou", "ze|zhe", "juan|xuan", "zan", "lei", "shen", "wei", \
	"chan", "li", "yi|tui", "bian", "zhe", "yan", "e", "chou", "wei", "chou", \
	"yao", "chan", "rang", "yin", "lan", "chen|chan", "xie", "nie", "huan", "zan", \
	"yi", "dang", "zhan", "yan", "du", "pianpang|yan", "ji", "ding", "fu", "ren", \
	"ji", "ji|jie", "hong", "tao", "rang", "shan", "qi", "tuo", "xun", "yi", \
	"xun", "ji", "ren", "jiang", "hui", "ou|xu", "ju", "ya", "ne", "hu|xu", \
	"e", "lun", "xiong", "song", "feng", "she", "fang", "jue", "zheng", "gu", \
	"he", "ping", "zu", "shi|zhi", "xiong", "zha", "su", "zhen", "di|ti", "chao|chou|zhou", \
	"ci", "chu|qu", "zhao", "bi", "yi", "yi|dai", "kuang", "lei", "shi", "gua", \
	"shi", "jie|ji", "hui", "cheng", "zhu", "shen", "hua", "dan", "gou|hou", "quan", \
	"gui", "xun", "yi", "zheng", "gai", "xiang|yang", "cha|du", "hun", "xu", "chou|zhou", \
	"jie", "wu", "yu", "qiao", "wu", "gao", "you", "hui", "kuang", "shuo|shui", \
	"song", "ei|xi", "qing", "chu|zhu", "zhou|zou", "nuo", "du|dou", "zhuo", "fei", "ke", \
	"wei", "yu", "shui|shei", "mie|shen", "diao|tiao", "chan", "liang", "zhun", "sui", "tan", \
	"shen", "yi", "mou", "chen", "die|xie", "huang", "jian|lan", "xie", "nue|xue", "ye", \
	"wei", "e", "tou|yu", "xuan", "chan", "zi", "an|tou", "yan", "di|ti", "mi|mei", \
	"pian", "xu", "mo", "dang", "su", "xie", "yao", "bang", "shi|xi|yi", "qian|zhan", \
	"mi", "jin", "man", "ze|zhe", "jian", "miu", "tan", "jian|zen", "qiao", "lan", \
	"pu", "jue", "yan", "qian", "zhan", "chan|chen", "gu|yu", "qian", "hong", "xia", \
	"ji", "hong", "han", "hong|long", "xi|ji", "xi", "huo|hua", "liao", "han|gan", "du", \
	"long", "dou", "jiang", "kai|qi", "chi", "feng|li", "deng", "wan", "bi|bian", "shu", \
	"xian", "feng", "zhi", "zhi", "yan", "yan", "shi", "chu", "hui", "tun", \
	"yi", "dun|tun", "yi", "jian|yan", "ba", "hou", "e", "chu", "xiang", "huan", \
	"jian|yan", "ken|kun", "gai", "ju", "fu|pu", "xi", "huan|bin", "hao", "shu|xie|yu", "zhu", \
	"jia", "fen", "xi", "hu|bo", "wen", "huan", "ban|bin", "di", "zong", "fen", \
	"yi", "zhi", "bao", "chai", "an", "pi", "na", "pi", "gou", "duo|na", \
	"you", "diao", "mo", "si", "xiu", "huan", "ken|kun", "he|mo", "he|mo|hao", "ma|mo", \
	"an", "mao|mo", "li|mai", "ni", "bi", "yu", "jia", "tuan", "mao", "pi", \
	"xi", "yi", "lou|yu|ju", "mo", "chu", "tan", "huan", "jue", "bei", "zhen", \
	"yun|yuan", "fu", "cai", "gong", "dai|te", "yi", "hang", "wan", "pin", "huo", \
	"fan", "tan", "guan|wan", "ze|zhai", "zhi", "er", "zhu", "shi", "bi", "zi", \
	"er", "gui", "pian", "bian|fa", "mai", "dai|te", "sheng", "kuang", "fei|fu", "tie", \
	"yi", "chi", "mao", "he", "ben|bi", "lu", "lin", "hui", "gai", "pian", \
	"zi", "jia|gu", "xu", "zei", "jiao", "gai", "zang", "jian", "ying", "xun", \
	"zhen", "sha|she", "bin", "bin", "qiu", "sha|she", "chuan", "zang", "zhou", "lai", \
	"zan", "ci", "chen", "shang", "tian", "pei", "geng", "xian", "mai", "jian", \
	"sui", "fu", "tan|dan", "cong", "cong", "zhi", "ji", "zhang", "du", "jin", \
	"xiong|min", "chun", "yun", "bao", "zai", "lai", "feng", "cang", "ji", "sheng", \
	"yi|ai", "zhuan|zuan", "fu", "gou", "sai", "ze", "liao", "yi", "bai", "chen", \
	"wan", "zhi", "zhui", "biao", "bin|yun", "zeng", "dan", "zan", "yan", "pu", \
	"dan|shan", "wan", "ying", "jin", "gan|gong", "xian", "zang", "bi", "du", "shu", \
	"yan", "uu", "xuan", "long", "gan|gong", "zang", "bei", "zhen", "fu", "yun|yuan", \
	"gong", "cai", "ze|zhai", "xian", "bai", "zhang", "huo", "zhi", "fan", "tan", \
	"pin", "bian|fa", "gou", "zhu", "guan|wan", "er", "jian", "ben|bi", "shi", "tie", \
	"gui", "kuang", "dai|te", "mao", "bi|fei|fu", "he", "yi", "zei", "zhi", "jia|gu", \
	"hui", "zi", "lin", "lu", "zang", "zi", "gai", "jin", "qiu", "zhen", \
	"lai", "sha|she", "fu", "du", "ji", "shu", "shang", "ci", "bi", "zhou", \
	"geng", "pei", "tan|dan", "lai", "feng", "zhui", "fu", "zhuan|zuan", "sai", "ze", \
	"yan", "zan", "bin|yun", "zeng", "dan|shan", "ying", "gan|gong|zhuang", "chi", "xi", "ce|she", \
	"nan", "tong|xiong", "xi", "cheng", "he|shi", "cheng", "zhe", "xia", "tang", "zou", \
	"zou", "li", "jiu", "fu", "diao|zhao", "gan|qian", "qi", "shan", "qiong", "qin|yin", \
	"xian", "ci|zi", "jue|gui", "qin", "di|chi", "ci", "chen|nian|zhen", "chen|zhen", "die|tu", "qie|ju", \
	"chao|tiao", "di", "xi", "zhan", "jue|ju", "huo|yue", "cou|cu|qu", "jie|ji", "chi|qu", "chu", \
	"gua|huo", "xue|chi", "ci|zi", "tiao", "duo", "lie", "gan", "suo", "cu", "xi", \
	"zhao|diao", "su", "yin", "ju|qiu|qu", "jian", "que|ji|qi", "cheng|tang|zheng", "chuo|chao", "wei|cui", "lu", \
	"cou|cu|qu", "dang", "qiu|cu", "zi", "ti", "qu|cu", "chi", "huang|guang", "qiao|jiao|chao", "qiao", \
	"jiao", "zao", "yao|ti|yue", "er", "zan|zu", "zan|zu", "ju|zu", "pa", "bao|bo", "wu|ku|kua", \
	"ke", "dun", "jue|gui", "fu", "chen", "jian|yan", "fang|pang", "zhi", "qi|ta", "yue", \
	"ba|pa|pao", "ji|qi", "ti|yue", "qiang", "chi|tuo", "tai", "yi", "chen|jian|nian", "ling", "mei", \
	"ba|bei", "die|tu", "ku", "tuo", "jia", "ci|zi", "bo|pao", "qia", "zhu", "qu|ju", \
	"zhan|die|tie", "zhi", "fu", "ban|pan", "qu|ju", "shan", "bi|bo|po", "ni", "ju", "li|luo", \
	"gen", "yi", "ji", "duo|dai", "sun|xian", "jiao|qiao", "duo", "zhu|chu", "quan|zun", "ku|kua", \
	"zhuai|shi", "gui", "qiang|qiong", "kui|xie", "xiang", "die|chi", "lu|luo", "pian|beng", "zhi", "jia|jie", \
	"diao|tao|tiao", "cai", "jian", "da", "qiao", "bi", "xian", "duo", "ji", "ju|qu", \
	"ji", "shu|chou", "tu|duo", "cu|chu", "jing|keng", "nie", "qiao|xiao", "bu", "chi|xue", "cun|zun", \
	"mu", "shu", "lang|liang", "yong", "jiao", "chou", "qiao", "meo", "ta", "jian", \
	"qi|ji", "wo|wei", "cu|wei", "chuo|diao|tiao|zhuo", "jie", "ji|qi", "nie", "ju", "nie", "lun", \
	"lu", "cheng|leng", "huai", "ju", "chi", "wan|wo", "juan|quan", "die|ti", "bo|pou", "cu|zu", \
	"qie", "qi|ji", "cu|di", "zong", "cai|kui", "zong", "pan|peng", "zhi", "zheng", "dian|die", \
	"zhi", "yu|yao", "chuo|duo", "dun", "chun|chuan", "yong", "zhong", "di|chi", "zha", "chen", \
	"chuai|chuan|duan|shuan", "jian", "gua|tuo", "shang|tang", "ju", "fu|bi", "cu|zu", "die", "pian", "rou", \
	"na|nuo|re", "di|ti", "cha|zha", "tui", "jian", "dao", "cuo", "qi|xi", "ta", "qiang", \
	"nian|zhan", "dian", "ti|di", "ji", "nie", "liang|man|pan", "liu", "zan|can", "bi", "chong", \
	"lu", "liao", "cu", "tang", "dai|die", "su", "xi", "kui", "ji", "zhi|zhuo", \
	"qiang", "di|zhi", "liang|man|pan", "zong", "lian", "beng|cheng", "zao", "ran|nian", "bie", "tui", \
	"ju", "deng", "ceng", "xian", "fan", "chu", "zhong|chong", "dun|cun", "bo", "cu|jiu", \
	"cu", "gui|jue", "jue", "lin", "ta", "qiao", "qiao|jiao", "pu", "liao", "dun", \
	"cuan", "guan", "zao", "da", "bi", "bi", "zhu|zhuo", "ju", "chu|chuo", "qiao", \
	"dun", "chou", "ji", "wu", "yue|ti", "nian", "lin", "lie", "zhi", "li|yue|luo", \
	"zhi", "chan|zhan", "chu", "duan", "wei", "long", "lin", "xian", "wei", "cuo|zuan", \
	"lan", "xie", "rang", "xie|sa", "nie", "ta", "qu", "ji|qi", "cuan", "zuan|cuo", \
	"xi", "kui", "jue|qi", "lin", "juan|shen", "gong", "dan", "fen", "qu", "ti", \
	"duo", "duo", "gong", "lang", "ren", "luo", "ai", "ji", "ju", "tang", \
	"kong", "uu", "yan", "mei", "kang", "qu", "lou|lv", "lao", "duo|tuo", "zhi", \
	"yan", "ti", "dao", "ying", "yu", "che|ju", "ya|zha|ga", "gui", "jun", "wei", \
	"yue", "xin|xian", "dai", "xuan|xian", "fan|gui", "ren", "shan", "kuang", "shu", "tun", \
	"chen|qi", "tai|dai", "e", "na", "qi", "mao", "ruan", "kuang", "qian", "zhuan", \
	"hong", "hu", "gou|qu", "kuang", "di|chi", "ling", "dai", "ao", "zhen", "fan", \
	"kuang", "yang", "peng", "bei", "gu", "gu", "pao", "zhu", "fu|rong", "e", \
	"ba", "zhou", "zhi", "diao|yao", "ke", "die|yi|zhe", "qing|zhi", "shi", "peng|ping", "er", \
	"gong", "ju", "jiao|jue|xiao", "guang", "he|lu|ya", "kai", "chun|quan", "zhou", "zai|zi", "zhi", \
	"she", "liang", "yu", "shao", "you", "huan|wan", "qun|yin", "zhe", "wan", "fu", \
	"qing", "zhou", "ni|yi", "ling|leng", "zhe", "zhan", "liang", "zi", "hui", "wang", \
	"chuo", "guo|hua", "kan", "yi", "peng", "qian", "gun", "nian", "peng|ping", "guan", \
	"bei", "lun", "pai", "liang", "er|ruan", "rou", "ji", "yang", "xian|kan", "chuan", \
	"cou", "shun|chun", "ge|ya", "you", "hong", "shu", "fu|bu", "zi", "fu", "wen|yun", \
	"fan|ben", "nian|zhan", "yu", "wen|yun", "tao|kan", "gu", "zhen", "xia|he", "yuan", "lu", \
	"xiao|jiao", "chao", "zhuan|zhuai", "wei", "hun|xuan", "xue", "zhe", "jiao", "zhan", "bu", \
	"lao|liao", "fen", "fan", "lin", "ge", "se", "kan", "huan", "yi", "ji", \
	"zhui", "er", "yu", "jian", "hong", "lei", "pei", "li", "li", "lu", \
	"lin", "che|ju", "ya|zha|ga", "gui", "han|xian|xuan", "dai", "ren", "zhuan|zhuai", "e", "lun", \
	"ruan", "hong", "gu", "ke", "lu", "zhou", "zhi", "die|yi|zhe", "hu", "zhen", \
	"li", "diao|yao", "qing", "shi", "zai|zi", "zhi", "jiao", "zhou", "chun|quan", "he|lu|ya", \
	"jiao|jue|xiao", "zhe", "fu", "liang", "nian", "bei", "hui", "gun", "wang", "liang", \
	"chuo", "zi", "cou", "fu", "ji", "wen|yun", "shu", "pei", "yuan", "he|xia", \
	"zhan|nian", "lu", "zhe", "lin", "xin", "gu", "ci", "ci", "pi|bi", "zui", \
	"bian", "la", "la", "ci", "xue|yi", "ban|bian", "ban|bian", "ban|bian", "ban|bian|pian", "uu", \
	"bian", "ban", "ci", "bian", "bian|pian", "chen", "ru", "nong", "nong", "zhen", \
	"zou|chuo", "zou", "yi", "reng", "bian", "bian|dao", "shi", "ru", "liao", "da|ta|ti", \
	"chan", "gan", "qian", "yu", "yu", "qi", "xun", "yi|tuo", "guo", "mai", \
	"qi", "bi|za", "wang|kuang|guang", "tu", "zhun", "ying", "ti|da", "yun", "jin", "hang|xiang", \
	"ya", "fan", "wu", "ti|da", "e", "huan|hai", "zhe|zhei", "zhong", "jin", "yuan", \
	"hui|wei", "lian", "chi|zhi", "che", "ni|chi", "tiao", "chi|zhi|li", "tuo|yi", "jiong", "jia|xie", \
	"chen|zhen", "dai", "er", "di", "po|pai", "zhu|wang", "da|die|yi", "ze", "tao", "shu", \
	"tuo|yi", "keop|qu", "jing", "hui", "dong", "you", "mi", "beng|peng", "ji", "nai", \
	"yi", "jie", "dui|tui|zhui", "lie", "xun", "tui", "song", "kuo|shi", "tao", "feng|pang", \
	"hou", "ni", "dun", "jiong", "xuan|shua|suan", "xun", "bu", "you", "xiao", "qiu", \
	"shu|tou", "di|tun|zhou|zhu", "qiu", "di", "di", "tu", "jing", "ti", "dou|qi|tou|zhu", "yi|si", \
	"zhe|zhei", "tong", "guang|kuang", "wu", "shi", "cheng|ying", "su", "cao|zao", "qun|suo|xun", "feng|pang|peng", \
	"lian", "suo", "hui", "li", "gu", "lai", "ben", "cuo", "zhu|jue", "beng|peng", \
	"huan", "dai|di", "dai|lu", "you", "zhou", "jin", "yu", "chuo", "kui", "wei", \
	"ti", "yi", "da|ta", "yuan", "luo", "bi", "nuo", "dou|yu", "dang|tang", "sui", \
	"dun|qun|xun", "sui", "an|yan", "chuan", "chi", "ti", "ou|yong|yu", "shi", "zhen", "you", \
	"yun", "e", "bian", "guo|huo", "e", "xia", "huang", "qiu", "dao", "da|ta", \
	"hui|wei", "nan", "yi|wei", "gou", "yao", "chou", "liu", "xun", "ta", "di|shi", \
	"chi|xi|zhi", "yuan", "su", "ta", "qian", "hweong|ma", "yao", "guan", "zhang", "ao", \
	"di|shi|ti|zhe", "ca", "chi", "su", "zao", "zhe", "dun", "shi|di", "lou", "chi|zhi", \
	"cuo", "lin", "zun", "rao", "qian", "xuan|suan|shua", "yu", "yi|wei", "e", "liao", \
	"ju|qu", "shi", "bi", "yao", "mai", "xie", "sui", "huan|hai|xuan", "zhan", "teng", \
	"er", "miao", "bian", "bian", "la|lie", "li|chi", "yuan", "yao|you", "luo", "li", \
	"e|yi", "ting", "deng|shan", "qi", "yong", "shan", "han", "yu", "mang", "ru|fu", \
	"qiong", "wan", "kuang|kuo", "fu", "hang|kang", "bin", "fang", "geng|xing", "na|nei|ne", "xin", \
	"shen", "bang", "yuan", "cun", "huo", "xie|ye", "bang", "wu", "ju", "you", \
	"han", "tai", "qiu", "bi|bian", "pi", "bing", "shao", "bei", "wa", "di", \
	"ju|zou", "qiu|ye", "lin", "kuang", "gui", "zhu", "shi", "ku", "yu", "hai|gai", \
	"he|xia", "xi|qie", "ji|zhi", "ji", "xun|huan", "hou", "geng|xing", "jiao", "xi", "gui", \
	"fu|na|nuo", "lang", "jia", "kuai", "zheng", "lang", "yun", "yan", "cheng", "dou", \
	"xi|chi", "lv", "fu", "wu|yu", "fu", "gao", "hao|shi", "lang", "jia", "geng", \
	"jun", "cheng|ying", "bo", "xi", "qu|ju", "li|zhi", "yun", "bu|pou", "ao|xiao", "qi", \
	"pi", "qing", "guo", "zhou", "tan", "zou|ju", "ping", "lai|lei", "ni", "chen|lan", \
	"chui|you", "bu|pou", "xiang", "dan|duo", "ju", "yong", "qiao", "yi", "dou|du", "yan", \
	"mei", "ruo", "bei", "e", "shu", "juan", "yu", "yun", "hou", "kui", \
	"xiang", "xiang", "sou", "tang", "ming", "xi", "ru", "chu", "jin|zi", "zou|ju", \
	"ye", "wu", "xiang", "yun", "hao|jiao|qiao", "yong", "bi", "mao", "chao", "lu|fu", \
	"liao", "yin", "zhuan", "hu", "qiao", "yan", "zhang", "man|wan", "qiao", "xu", \
	"deng", "bi", "xun", "bi", "zeng", "wei", "zheng", "mao", "shan", "lin", \
	"pan|pi|po", "dan|duo", "meng", "ye", "cao|sao", "kuai", "feng", "meng", "ju|zou", "kuang|kuo", \
	"lian", "zan|cuo", "chan", "you", "ji|qi", "yan", "chan", "zan|cuo", "ling", "quan|huan", \
	"xi", "feng", "zan|cuo", "li|zhi", "you", "ding", "qiu", "zhuo", "pei", "zhou", \
	"yi", "gan|hang", "yu", "jiu", "yin|yan", "zui", "mao", "dan|zhen", "xu", "dou", \
	"zhen", "fen", "yuan", "fu", "yun", "tai", "tian", "qia", "dou|tuo", "zuo|cu", \
	"han", "gu", "su", "po|fa", "chou", "zai|zui", "ming", "lao|lu", "chuo", "chou", \
	"you", "chong|dong|tong", "zhi", "xian", "jiang", "cheng", "yin", "tu", "jiao", "mei", \
	"ku", "suan", "lei", "pu", "fu|zui", "hai", "yan", "shi|shai", "niang", "wei|zhui", \
	"lu", "lan", "yan|ang", "tao", "pei", "zhan", "chun", "tan|dan", "zui", "zhui", \
	"cu|zuo", "kun", "ti", "xian|jian", "du", "hu", "xu", "cheng|jing|xing", "tan", "chou|qiu", \
	"chun", "yun", "fa", "ke", "sou", "mi", "chuo|quan", "chou", "cuo", "yun", \
	"yong", "ang", "zha", "hai", "tang", "jiang", "piao", "chan|chen", "ou|yu", "li", \
	"zao", "lao", "yi", "jiang", "bu", "jiao|qiao|zhan", "xi", "tan", "fa|po", "nong", \
	"yi|shi", "li", "ju", "lian|yan", "yi|ai", "niang", "ru", "xun", "chou|shou", "yan", \
	"ling", "mi", "mi", "niang", "xin", "jiao", "shi|shai", "mi", "yan", "bian", \
	"cai", "shi", "you", "shi|yi", "shi|yi", "li", "zhong|chong", "shu|ye", "liang", "li|xi", \
	"jin", "jin", "ga|qiu", "yi", "liao", "dao", "zhao", "ding|ling", "po", "qiu", \
	"he|ba", "fu", "zhen", "zhi", "ba", "luan", "fu", "nai", "diao", "shan|xian", \
	"qiao|jiao", "kou", "chuan", "zi", "fan", "hua|yu", "wu|hua", "gan|han", "gang", "qi", \
	"mang", "ren|jian|ri", "di", "si", "xi", "yi", "chai|cha", "shi|yi", "tu", "xi", \
	"nv", "qian", "qiu", "jian", "pi|zhao", "ya|ye", "jin|yin", "ba|pa", "fang", "chen|qin", \
	"xing|jian", "dou|tou", "yao|yue", "zhong|qian", "fu", "pi|bu", "na|rui", "qin|xin", "e", "jue", \
	"dun", "gou", "yin", "han|qian", "ban", "xi|sa", "ren", "chao|miao", "chou|niu", "fen", \
	"dui|yun", "yi", "qin", "bi|pi", "guo", "hong", "yin", "jun", "diao", "yi", \
	"zhong", "xi", "gai", "ri", "huo", "tai", "kang", "yuan", "lu", "ngag", \
	"wen", "duo", "zi", "ni", "tu", "shi", "min", "gu|pi", "ke", "ling", \
	"bing", "si|ci", "gu|hu", "bo", "pi", "yu", "si", "zuo", "bu", "you|zhou", \
	"dian|tian", "ge|jia", "zhen", "shi", "shi|zu", "tie|zhi", "ju", "zuan|qian|chan", "yi|shi", "tuo|ta", \
	"xuan", "zhao", "bao|pao", "he", "bi|se", "sheng", "zu|ju|chu", "shi|zu", "bo", "zhu", \
	"chi", "za", "po", "tong", "an|qian", "fu", "zhai", "liu|mao", "qian|yan", "fu", \
	"li", "yue", "pi", "yang", "ban", "bo", "jie", "gou|qu", "shu|xu", "zheng", \
	"mu", "ni|nie|xi", "nie|xi", "di", "jia", "mu", "tan", "huan|shen", "yi", "si", \
	"kuang", "ka", "bei", "jian", "zhuo|tong", "xing", "hong", "jiao", "chi", "er|keng", \
	"ge|luo", "bing|ping", "shi", "mou|mao", "jia|ha", "yin", "jun", "zhou", "chong", "xiang|jiong", \
	"tong", "mo", "lei", "ji", "si|yu", "xu|hui", "ren", "zun", "zhi", "qiong", \
	"shan|shuo", "chi|li", "xi|xian", "jian|xing", "quan", "pi", "yi|tie", "zhu", "hou|xiang", "ming", \
	"kua", "yao|tiao|diao", "kuo|gua|tian|xian", "xian", "xiu", "jun", "cha", "lao", "ji", "pi", \
	"ru", "mi", "yi", "yin", "guang", "an", "diu", "you", "se", "kao", \
	"jian|qian", "luan", "si", "ngai", "diao", "han", "dui|rui|yue", "zhi|shi", "keng", "qiu", \
	"xiao", "zhe|nie", "xiu|you", "zang", "ti", "cuo", "gua", "gong|hong", "yong|zhong", "dou|tou", \
	"lv", "mei|meng", "lang", "wan|jian", "xin|zi", "jun|yun", "bei", "wu", "su", "yu", \
	"chan", "ting|ding", "bo", "han", "jia", "hong", "cuan|juan", "feng", "chan", "wan", \
	"zhi", "si|tuo", "juan|xuan", "wu|hua", "wu|yu", "tiao", "kuang", "chuo|zhuo", "lue", "jing|xiang|xing", \
	"jin|qian|qin", "shen", "han", "lue", "ye", "chu|ju", "zeng", "ju", "xian", "e|tie", \
	"mang", "pu", "li", "pan", "dui|rui|yue", "cheng", "gao", "li", "te", "bing", \
	"zhu", "zhen", "tu", "liu", "zui|nie", "ju", "chang", "wan|yuan", "jian", "gang", \
	"diao", "tao", "chang|shang", "lun|fen", "guo|ke|kua", "ling", "pi", "lu", "li", "qing|qiang", \
	"fu|pei|pou", "juan", "min", "zu|zui", "peng|beng", "an", "bei|bi|pi", "xian|qian", "ya", "zhui", \
	"lei|li", "a|ke", "kong", "ta", "kun|gun", "du", "wei|nei", "chui", "zi", "zheng", \
	"ben", "nie", "zong", "chun|dui", "tan|xian|tian", "ding", "qi|yi", "jian|qian", "zhui|chuo", "ji", \
	"yu", "jin", "guan", "mao", "chang", "tian|tun", "ti|xi", "lian|jian", "diao|tao", "gu", \
	"cuo|cu", "shu", "zhen", "lu|lv", "meng", "lu", "hua", "biao", "ga", "lai", \
	"ken", "fang", "wu", "nai", "wan|jian", "zan", "hu", "de", "xian", "uu", \
	"huo", "liang", "fa", "men", "jie|kai", "ying|yang", "chi|di", "lian|jian", "guo", "xian", \
	"du", "tu", "wei", "zong|wan", "fu", "rou", "ji", "e", "jun", "chen|zhen", \
	"ti", "zha", "hu", "yang", "duan", "xia", "yu", "keng", "sheng", "huang", \
	"wei", "fu", "zhao", "cha", "qie", "shi", "hong", "kui", "nuo|tian", "mou", \
	"qiao", "qiao", "hou", "tou", "zong", "huan", "ye|xie", "min", "jian", "duan", \
	"jian", "si|song", "kui", "hu", "xuan", "zhe|du", "jie", "qian|zhen", "bian", "zhong", \
	"zi", "xiu", "ye", "mei", "pai", "ai", "gai", "qian", "mei", "suo|cha", \
	"ta|da", "pang|bang", "xia", "lian", "suo|se", "kai", "liu", "yao|zu", "ta|ye", "hao|nou", \
	"weng", "rong", "tang", "suo", "qiang|cheng", "ge|li", "shuo", "chui|dui", "bo", "pan", \
	"da|sa", "bi|pi", "sang", "gang", "zi", "wu", "jiong|ying", "huang", "tiao", "liu", \
	"kai", "sun", "se|sha|shi", "sou", "wan|jian", "gao|hao", "tian|zhen", "tian|zhen", "lang|luo", "yi", \
	"yuan", "tang", "nie", "xi", "jia", "ge", "ma", "juan", "song|ka", "zu|ha", \
	"suo", "uu", "feng", "wen", "na", "lu", "suo", "kou", "zu|chuo", "tuan", \
	"xiu", "guan", "xuan", "lian", "sou|shou", "ao", "man", "mo", "luo", "bi", \
	"wei", "liao|liu", "di", "can|qiao|san", "zong", "yi", "ao|lu", "ao|biao", "keng", "qiang", \
	"cui", "qi", "chang", "tang", "man", "yong", "chan", "feng", "jing", "biao", \
	"shu", "lou|lv", "xiu", "cong", "long", "zan", "zan|jian", "cao", "li", "xia", \
	"xi", "kang", "shuang", "beng", "zhang", "qian", "cheng", "lu", "hua", "ji", \
	"pu", "sui|hui", "qiang", "po", "lin", "se", "xiu", "san|sa|xian", "cheng", "kui", \
	"si", "liu", "nao", "huang", "pie", "sui", "fan", "qiao", "quan", "yang", \
	"tang", "xiang", "yu|jue", "jiao", "zun", "liao", "qie|qi", "lao", "dun|dui", "tan|xin", \
	"zan", "ji|qi", "jian", "zhong", "deng", "ya", "ying", "dun|dui", "jue", "hao|nou", \
	"ti|zan", "pu", "tie|die|te", "uu", "zhang", "ding", "shan", "kai", "jian", "fei", \
	"sui", "lu", "juan", "hui", "yu", "lian", "zhuo", "sao|qiao", "jian|qian", "shu|zhuo", \
	"lei", "bi|bei", "die|tie", "huan|xuan", "ye|xie", "duo", "guo", "dang|tang|cheng", "ju|qu", "ben|fen", \
	"da", "bei|bi", "yi", "ai", "zong", "xun", "diao", "zhu", "heng", "zhui", \
	"ji", "ni|nie", "he", "huo", "qing", "bin", "ying", "kui", "ning", "xu|ru", \
	"jian", "jian", "qian", "cha", "zhi", "mie|mi", "li", "lei", "ji", "zuan", \
	"kuang", "shang", "peng", "la", "du", "li|shuo|yue", "chuo", "lv", "biao", "bao", \
	"lu", "xian|zhi", "kuan", "long", "e", "lu", "xin|xun", "jian", "lan", "bo", \
	"qian|jian", "yao|yue", "chan", "xiang|rang", "jian", "xi", "guan", "cang", "nie", "lei", \
	"cuan", "qu", "pan", "luo", "zuan", "luan", "zao|zuo", "nie|yi", "jue", "tang", \
	"zhu", "lan", "jin|pianpang", "ga|qiu", "yi", "zhen", "ding|ling", "zhao", "po", "liao", \
	"tu", "qian", "chuan", "shan|xian", "xi|sa", "fan", "diao", "men", "nv", "yang", \
	"cha|chai", "xing|jian", "gai", "bu|pi", "tai", "ju", "dun", "chao", "zhong", "na|rui", \
	"bei", "gang", "ban", "han|qian", "yao|yue", "qin", "jun", "wu", "gou", "kang", \
	"fang", "huo", "dou|tou", "chou|niu", "ba|pa", "yu", "jian|qian", "zheng", "an|qian", "gu|hu", \
	"bo", "ke", "po", "bu", "bo", "yue", "zuan", "mu", "tan", "ge|jia", \
	"tian|dian", "you|zhou", "tie|zhi", "bo", "ling", "li|shuo|yue", "qian|yan", "liu|mao", "bao|pao", "shi", \
	"xuan", "ta|tuo", "bi|se", "ni", "pi", "duo", "xing", "kao", "lao", "er|keng", \
	"mang", "ya", "you", "cheng", "jia", "ye", "nao", "zhi", "dang|cheng", "tong", \
	"lv", "diao", "yin", "kai", "zha", "zhu", "xian|xi", "ting|ding", "diu", "kuo|gua|tian|xian", \
	"hua", "quan", "se|sha|shi", "ha|ke", "tiao|yao|diao", "ge|luo", "ming", "zheng", "se", "jiao", \
	"yi", "chan", "chong", "tang", "an", "yin", "ru", "zhu", "lao", "pu", \
	"wu|yu", "lai", "te", "lian", "keng", "xiao", "suo", "li", "zeng", "chu|ju", \
	"guo", "gao", "e|tie", "xiu|you", "cuo", "lue", "feng", "xin|zi", "liu", "kai", \
	"jian", "dui|rui|yue", "ti", "lang", "jin|qian|qin", "ju", "a", "qiang", "du|duo|zhe", "nuo|tian", \
	"cu|cuo|xi", "mao", "ben", "qi|yi", "de", "guo|ke|kua", "gun|kun", "chang", "ti|xi", "gu", \
	"luo", "chui", "zhui", "jin", "zhi", "xian", "juan", "huo", "fu|pei|pou", "tan|xian", \
	"ding", "jian", "ju", "meng", "zi", "qie", "ying|yang", "jie|kai", "qiang", "si|song", \
	"e", "cha", "qiao", "zhong", "duan", "sou", "huang", "huan", "ai", "du", \
	"mei", "lou|lv", "zi", "fei", "mei", "mo", "tian|zhen", "bo", "ge|li", "nie", \
	"tang", "juan", "nie", "na", "liu", "gao|hao", "bang|pang", "yi", "jia", "bin", \
	"rong", "biao", "tang", "man", "luo", "beng", "yong", "jing", "di", "zu|chuo", \
	"xuan", "liao|liu", "tan|chan|xin", "jue", "liao", "pu", "lu", "dun|dui", "lan|lian", "pu", \
	"chuan|cuan", "qiang", "deng", "huo", "lei", "huan|xuan", "shu|zhuo", "lian", "yi", "cha", \
	"biao", "la", "chan", "rang|xiang", "chang|zhang", "chang|zhang", "jiu", "ao", "die", "jue", \
	"liao", "mi|ni", "chang|zhang", "men", "ma", "shuan", "shan", "shan|huo", "men", "yan", \
	"bi", "han|bi", "bi", "shan", "kai|qian", "kang", "beng", "hong", "run", "san", \
	"xian", "xian|jian", "jian", "min", "xia", "shui", "dou", "zha|ya", "nao", "zhan", \
	"peng", "xia|e", "ling", "bian|guan", "bi", "run", "he|ai", "guan|wan", "ge", "he|ge", \
	"fa", "chu", "hong|xiang", "gui", "min", "seo", "kun", "lang|liang", "lv", "ting", \
	"sha", "ju", "yue", "yue", "chan", "qu", "lin", "chang|tang", "sha|shai", "kun", \
	"yan", "wen", "yan", "yu|e|yan", "hun", "yu", "wen", "xiang|hong", "bao", "juan|xiang|hong", \
	"qu", "yao", "wen", "pan|ban", "an|yin", "wei", "yin", "kuo", "jue|kui|que", "lan", \
	"du|she", "quan", "phdeng", "tian", "nie", "ta", "kai", "he", "jue|que", "chuang|chen", \
	"guan|wan", "dou", "qi", "kui", "tang|chang", "guan|wan", "piao", "kan|han", "xi|se", "hui", \
	"chan", "pi", "dang|tang", "huan", "ta", "wen", "uu", "men", "shuan", "shan", \
	"yan", "han|bi", "bi", "wen", "chen|chuang", "run", "wei", "xian", "hong", "jian", \
	"min", "kang", "men", "ge|ya|zha", "nao", "gui", "wen", "ta", "min", "lv", \
	"kai", "fa", "ge", "ai|gai|hai|he", "kun", "jiu", "yue", "lang|liang", "du|she", "yu", \
	"yan", "chang|tang", "he|xi", "wen", "hun", "yan", "yan|e", "chan", "lan", "qu", \
	"hui", "kuo", "jue|kui|que", "he", "tian", "ta", "jue|que", "kan|han", "huan", "fu", \
	"fu|pianpang", "le", "dui", "xin", "qian", "wei|wu", "gai|yi", "yi|tuo|zhi", "yin", "yang", \
	"dou", "e|ai", "sheng", "ban", "pei", "keng|gang", "yan|yun", "ruan|yuan", "zhi", "pi", \
	"jing", "fang", "yang", "lin|yin", "zhen", "jie", "cheng", "e|ai", "qu", "di", \
	"zhu|zu", "zuo", "yan|dian", "ling|lin", "a|e", "duo|tuo", "tuo|yi|zhi", "po|pi|bei", "bing", "bu|fu", \
	"ji", "lu|liu", "long", "chen", "jing|xing", "duo", "lou", "mo", "jiang|xiang", "shu", \
	"duo|sui", "wen|xian", "er", "gui", "yu", "gai", "shan", "jun", "qiao", "jing|xing", \
	"chun", "fu|wu", "bi", "xia", "shan", "sheng", "de|zhi", "pu|bu", "dou", "yuan", \
	"zhen", "chu|shu|zhu", "xian", "dao", "nie", "yuan|yun", "jian|xian|yan", "pei", "fei|pei", "zhe|zou", \
	"qi|yi", "dui", "lun", "an|yan|yin", "ju", "chui", "chen|zhen", "bi|pi", "ling", "tao|yao", \
	"xian", "lu|liu", "sheng", "xian", "yin", "du|zhu", "yang", "reng|er", "xia", "chong", \
	"yan", "an|yin", "yu|shu|yao", "di", "yu", "long", "wei", "wei", "nie", "dui|zhui", \
	"duo|sui|tuo", "an", "huang", "jie", "sui", "yin", "ai|gai", "yan", "duo|hui", "ge|ji|rong", \
	"yuan|yun", "wu", "wei|kui", "ai|e", "xi", "tang", "ji", "zhang", "dao", "ao", \
	"xi", "yin", "sa", "rao", "lin", "tui", "deng", "pi", "sui|zhui", "sui", \
	"yu", "xian|jian", "fen", "ni", "er", "ji", "dao", "xi|xie", "yin", "zhi", \
	"duo|hui", "long", "xi", "dai|di|li|yi", "li", "li", "cui|wei|zhui", "que|he", "huo|zhi", "sun", \
	"juan|jun", "nan|nuo", "yi", "que|qiao", "yan", "qin", "jie|qian", "xiong", "ya", "ji", \
	"gu|hu", "huan", "kai|si|yi|zhi", "gou", "jun|juan", "ci", "yong", "ju", "chu|ju", "hu", \
	"za", "luo", "yu", "chou", "diao", "sui", "han", "huo|wo", "shuang", "huan|guan", \
	"chu|ju", "za", "yong", "ji", "xi|gui", "chou", "liu", "chi|li", "nan|nuo", "xue", \
	"za", "ji", "ji", "yu", "xu|yu", "xue", "na", "fou", "xi|se", "mu", \
	"wen", "fen", "fang|pang", "yun", "li", "chi", "yang", "lian|ling", "lei", "an", \
	"bao", "wu|meng", "dian", "dang", "hu", "wu", "diao", "nuo|ruan|xu", "ji", "mu", \
	"chen", "xiao", "sha|zha", "ting", "shen|zhen", "pei", "mei", "ling", "qi", "zhou", \
	"he|huo|suo", "sha", "fei", "hong", "zhan", "yin", "ni", "zhu|shu", "tun", "lin", \
	"ling", "dong", "ying|yang", "wu|meng", "ling", "shuang", "ling", "xia", "hong", "yin", \
	"mai", "mai", "yun", "liu", "meng", "bin", "meng|wu", "wei", "kuo", "yin", \
	"xi", "yi", "ai", "dan", "teng", "san|xian", "yu", "lu|lou", "long", "dai", \
	"ji", "pang", "yang", "ba|po", "pi", "wei", "uu", "xi", "ji", "li|mai", \
	"mao|meng|wu", "meng", "lei", "li", "sui|huo", "ai", "fei", "dai", "long|ling", "ling", \
	"ai|yi", "feng", "li", "bao", "he", "he", "he", "bing", "jing|qing", "jing|qing", \
	"liang|jing", "tian", "zhen", "jing", "cheng", "jing|qing", "jing", "liang|jing", "dian", "jing", \
	"tian", "fei", "fei", "kao", "ma|mi", "mian", "mian", "bao", "yan|ye", "tian|mian", \
	"hui", "yan|ye", "ge|ji", "ding", "cha", "qian|jian", "ren", "di", "du", "wu", \
	"ren", "qin", "jin", "xue", "niu", "ba", "yin", "sa", "na", "mo|wa", \
	"zu", "da", "ban", "yi|xie", "yao", "tao", "bei|bi", "jie", "hong", "pao", \
	"yang", "bing", "yin", "ge|sa|ta", "tao", "ji|jie", "wa|xie", "an", "an", "hen", \
	"gong", "qia", "da|ta", "jue|qiao", "ting", "man|men", "bian|ying", "sui", "tiao", "qiao|shao", \
	"juan|xuan", "kong", "beng", "ta", "shang|zhang", "bi|bing", "kuo", "ju|qiong|qu", "la", "die|xie|zha", \
	"rou", "bang", "eng", "qiu", "qiu", "he|she", "qiao|shao", "mou|mu", "ju|qu", "jian", \
	"bian", "di", "jian", "on", "tao", "gou", "ta", "bai|bei|bu|fu", "xie", "pan", \
	"ge", "bi|bing", "kuo", "tang", "lou", "gui|hui", "jue|qiao", "xue", "ji", "jian", \
	"jiang", "chan", "ta|da", "hu", "xian", "qian", "du", "wa", "jian", "lan", \
	"hui|wei", "ren", "fu", "wa|mei", "juan|quan", "ge", "wei", "qiao|shao", "han", "chang", \
	"kuo", "rou|ruo", "yun", "she", "wei", "ge", "bai|fu", "tao", "gou|bei", "wen|yun", \
	"gao", "bi", "xue|wei", "sui|hui", "du", "wa", "du", "hui|wei", "ren", "fu", \
	"han", "wei", "wen|yun", "tao", "jiu", "jiu", "xian", "xie", "xian", "ji", \
	"yin", "za", "yun", "shao", "le", "peng", "huang|ying", "ying", "yun", "peng", \
	"an", "yin", "xiang", "hu", "ye|xie", "ding", "kui|qing", "qiu|kui", "xiang", "shun", \
	"han|an", "xu", "yi", "xu", "gu|e", "rong|song", "kui", "ken|qi", "gang|hang", "yu", \
	"wan|kun", "ban|fen", "dun|du", "di", "dan|dian", "pan", "pi|po", "ling", "che", "jing", \
	"lei", "he|ge|han", "qiao", "an|e", "e", "wei", "xie|jie", "kuo", "shen", "yi", \
	"yi", "hai|ke", "dui", "yu|bian", "ping", "lei", "fu|tiao", "jia", "tou", "pou|hui", \
	"kui", "jia", "luo", "ting", "cheng", "jing|ying", "yun", "hu", "han", "jing|geng", \
	"tui", "tui", "bin|pin", "lai", "tui", "zi", "zi", "chui", "ding", "lai", \
	"tan|shan", "han", "qian", "ke|kuan", "zu|cui", "xian|jiong", "qin", "yi", "sai", "di|ti", \
	"e", "e", "yan", "hun|wen", "yan|kan", "yu|yong", "zhuan", "ya|yan", "xian", "pi|xin", \
	"yi", "yuan", "sang", "dian|tian", "dian|tian", "jiang", "kua|kui", "lei", "lao", "piao", \
	"wai|zhuai", "man", "cu", "qiao|yao", "hao", "qiao", "gu", "xun", "yan|qin", "hui", \
	"zhan|chan|shan", "ru", "meng", "bin", "xian", "pin", "lu", "lin|lan", "nie", "quan", \
	"xie|ye", "ding", "kui|qing", "an|han", "xiang", "shun", "xu", "xu", "kun|wan", "gu", \
	"dun|du", "ken|qi", "ban|fen", "rong|song", "hang|gang", "yu", "lu", "ling", "pi|po", "jing|geng", \
	"jie|xie|jia", "jia", "ting", "he|ge", "ying", "jiong", "hai|ke", "yi", "bin|pin", "pou", \
	"tui", "han", "jing|ying", "ying", "ke|kuan", "ti|di", "yu|yong", "e", "zhuan", "ya|yan", \
	"e", "nie", "man", "dian|tian", "sang", "hao", "lei", "chan|zhan", "ru", "pin", \
	"quan", "feng", "diao|diu", "gua", "fu", "xia", "zhan", "biao|pao", "li|sa", "ba|fu", \
	"tai", "lie", "gua|ji", "xuan", "shao|xiao", "ju", "biao", "si", "wei", "yang", \
	"yao", "sou", "kai", "sou|sao", "fan", "liu", "xi", "liao|liu", "piao", "piao", \
	"liu", "biao", "biao", "biao", "liao", "biao", "se", "feng", "xiu", "feng", \
	"yang", "zhan", "biao|pao", "li|sa", "ju", "si", "sou", "yao", "liu", "piao", \
	"biao", "biao", "fei", "fan", "fei", "fei", "shi|si|yi", "shi", "can", "ji", \
	"ding", "si", "tuo", "gan|zhan", "sun", "xiang", "tun|zhun", "ren", "yu", "yang|juan", \
	"chi|shi", "yin", "fan", "fan", "can|sun", "yin", "zhu|tou", "yi|si", "zha|zuo|ze", "bi", \
	"jie", "tao", "bao", "ci", "tie", "si", "bao", "chi|shi", "duo", "hai", \
	"ren", "tian", "jiao", "he|jia", "bing", "yao", "tong", "ci", "xiang", "yang", \
	"juan", "er", "yan", "le", "xi", "can|sun", "bo", "nei", "e", "bu", \
	"jun", "dou", "su", "ye|yu", "xi|shi", "yao", "hun|kun", "guo", "chi|shi", "jian", \
	"zhui", "bing", "kan|xian", "bu", "ye", "dan|tan", "fei", "zhang", "wei|nei", "guan", \
	"e", "nuan", "hun|yun", "hu", "huang", "tie", "hui", "zhan|jian", "hou", "he|ai", \
	"tang|xing", "fen", "wei", "gu", "zha|cha", "song", "tang", "bo", "gao", "xi", \
	"kui", "liu", "sou", "tao|xian", "ye", "wen", "mo", "tang", "man", "bi", \
	"yu", "xiu", "jin", "san", "kui|tui", "xuan|zhuan", "shan", "xi|chi", "dan", "ye|yi", \
	"ji|qi", "rao", "cheng", "yong", "tao", "wei", "xiang", "zhan", "fen", "hai", \
	"meng", "yan", "mo", "chan", "xiang", "luo", "zan", "nang", "pianpang|shi", "ding", \
	"ji", "tuo", "tang|xing", "tun|zhun", "xi", "ren", "yu", "chi|shi", "fan", "yin", \
	"jian", "chi|shi", "bao", "si", "duo", "si|yi", "er", "rao", "xiang", "he|jia", \
	"le|ge", "jiao", "xi", "bing", "bo", "dou", "e", "ye|yu", "nei", "jun", \
	"guo", "hun|kun", "kan|xian", "guan", "zha|cha", "kui|tui", "gu", "sou", "chan", "ye", \
	"mo", "bo", "liu", "xiu", "jin", "man", "san", "xuan|zhuan", "nang", "shou", \
	"kui|qiu", "guo|xu", "xiang", "fen", "bo", "ni", "bi", "bo|po", "tu", "han", \
	"fei", "jian", "an", "ai", "bi|fu", "xian", "yun|wo", "xin", "fen", "pin", \
	"xin", "ma", "yu", "feng|ping", "han|qian", "di", "tuo|duo", "tuo|zhe", "chi", "xun", \
	"zhu", "zhi|shi", "pei", "jin|xin", "ri", "sa", "yun", "wen", "zhi", "dan", \
	"lv|lu", "you", "bo", "bao", "jue|kuai", "tuo|duo", "yi", "qu", "pu", "qu", \
	"jiong", "po", "zhao", "yuan", "pei|peng", "zhou", "ju", "zhu", "nu", "ju", \
	"pi", "zu|zang", "jia", "ling", "zhen", "tai|dai", "fu", "yang", "shi", "bi", \
	"tuo", "tuo", "si", "liu", "ma", "pian", "tao", "zhi", "rong", "teng", \
	"dong", "xun|xuan", "quan", "shen", "jiong", "er", "hai", "bo", "zhu", "yin", \
	"luo|jia", "zhou", "dan", "hai", "liu", "ju", "song", "qin", "mang", "liang|lang", \
	"han", "tu", "xuan", "tui", "jun", "e", "cheng", "xing", "si|ai", "lu", \
	"zhui", "zhou|dong", "she", "pian", "kun", "tao", "lai", "zong", "ke", "qi|ji", \
	"qi", "yan", "fei", "sao", "yan", "ge", "yao", "wu", "pian", "cong", \
	"pian", "qian", "fei", "huang", "qian", "huo", "yu", "ti", "quan", "xia", \
	"zong", "jue|kui", "rou", "si", "gua", "tuo", "gui|tui", "sou", "qian|jian", "cheng", \
	"zhi", "liu", "bang|peng", "teng", "xi", "cao", "du", "yan", "yuan", "zou|zhu", \
	"sao|xiao", "shan", "qi", "zhi|chi", "shuang", "lu", "xi", "luo", "zhang", "mo|ma", \
	"ao|yao", "can", "biao|piao", "cong", "qu", "bi", "zhi", "yu", "xu", "hua", \
	"bo", "su", "xiao", "lin", "zhan", "dun", "liu", "tuo", "ceng", "dian", \
	"jiao|ju|qiao|xiao", "tie", "yan", "luo", "zhan", "jing", "yi", "ye", "zhe|tuo", "pin", \
	"zhou", "yan", "long|zang", "lv", "teng", "xiang", "ji", "shuang", "ju", "xi", \
	"huan", "chi|li", "biao|piao", "ma", "yu", "tuo|duo", "xun", "chi", "qu", "ri", \
	"bo", "lv", "zang|zu", "shi", "si", "fu", "ju", "qu|zhou|zhu|zou", "zhu", "tuo", \
	"nu", "jia", "yi", "tai|dai", "xiao", "ma", "yin", "jiao|ju|qiao|xiao", "hua", "jia|luo", \
	"hai", "pian", "biao|piao", "chi|li", "cheng", "yan", "xing", "qin", "jun", "qi", \
	"qi|ji", "ke", "zhui", "zong", "su", "can", "pian", "zhi", "jue|kui", "sao|xiao", \
	"wu", "ao|yao", "liu", "jian|qian", "shan", "biao|piao", "luo", "cong", "chan|zhan", "zhou", \
	"ji", "shuang", "xiang", "gu", "wan|wei", "wan|wei", "wan|wei", "yu", "gan", "yi", \
	"ang|kang", "gu|tou", "jia|jie|xie", "bao", "bei", "ci|zhai", "ti", "di", "ku", "gai|hai", \
	"jiao|qiao", "hou", "kua", "ge", "tui", "geng", "pian", "bi", "ke|kua", "ge|qia", \
	"yu|ou", "sui", "lou", "bo|po", "xiao", "pang|bang", "jue|bo", "cuo|ci", "kuan", "bin", \
	"mo", "liao", "lou", "xiao", "du", "zang", "sui", "ti", "bin", "kuan", \
	"lu", "gao", "gao", "qiao", "kao", "qiao", "lao", "sao", "biao|piao", "kun", \
	"kun", "ti|di", "fang", "xiu", "ran", "mao", "dan", "kun", "bin", "fa", \
	"jie|tiao", "pi", "zi", "fa", "ran", "ti|di", "bao", "bi|po", "mao|meng|rou", "fei|fu", \
	"er", "rong|er", "qu", "gong", "xiu", "kuo|yue", "ji", "peng", "zhua", "shao", \
	"sha|suo", "ti", "li", "bin", "zong", "ti|di", "peng", "song", "zheng", "quan", \
	"zong", "shun", "jian", "chui|duo", "hu", "la", "jiu", "qi", "lian", "zhen", \
	"bin", "peng", "ma", "san", "man", "man", "seng", "xu", "lie", "qian", \
	"qian", "nang", "huan", "kuai|kuo", "ning", "bin", "lie", "rang|ning", "dou", "dou", \
	"nao", "xiang|hong", "he|xi", "dou", "han", "dou", "dou", "jiu", "chang", "yu", \
	"yu", "ge|li", "yan", "fu|li", "xin|qin", "gui", "zeng|zong", "liu", "gui|xie", "shang", \
	"ju|yu|zhou", "gui", "mei", "qi|ji", "qi", "ga", "kuai|kui", "hun", "ba", "po|bo|tuo", \
	"mei", "xu", "yan", "xiao", "liang", "yu|huo", "zhui|tui", "qi", "wang", "liang", \
	"wei", "gan", "chi", "piao", "bi", "mo", "qi", "xu", "chou", "yan", \
	"zhan", "yu", "dao", "ren", "ji|jie", "ba", "hong|gong", "tuo", "diao|di", "ji", \
	"xu|yu", "hua|e", "ji|e|qie", "sha|suo", "hang", "tun", "mo", "jie", "shen", "ban", \
	"yuan|wan", "bi|pi", "lu|lv", "wen", "hu", "lu", "shi|za", "fang", "fen", "na", \
	"you", "pian", "mo", "he|ge", "xia", "qu|xie", "han", "pi", "ling|lin", "tuo", \
	"ba|bo", "qiu", "ping", "fu", "bi", "ci|ji", "wei", "ju|qu", "diao", "bo|ba", \
	"you", "gun", "ju|pi", "nian", "xing|zheng", "tai", "bao|pao", "fu", "zha", "ju", \
	"gu", "shi", "dong", "chou|dai", "ta|die", "jie|qia", "shu", "hou", "xiang|zhen", "er", \
	"an", "wei", "zhao", "zhu", "yin", "lie", "luo|ge", "tong", "yi|ti", "yi|qi", \
	"bing|bi", "wei", "jiao", "ku", "gui|xie", "xian", "ge", "hui", "lao", "fu", \
	"kao", "xiu", "duo", "jun", "ti", "mian", "shao", "zha", "suo", "qin", \
	"yu", "nei", "zhe", "gun", "geng", "su", "wu", "qiu", "shan|shen", "pu|bu", \
	"huan", "tiao|chou", "li", "sha", "sha", "kao", "meng", "cheng", "li", "zou", \
	"xi", "yong", "shen", "zi", "qi", "qing|zheng", "xiang", "nei", "chun", "ji", \
	"diao", "qie", "gu", "zhou", "dong", "lai", "fei", "ni", "yi", "kun", \
	"lu", "ai|jiu", "chang", "jing", "lun", "ling", "zou", "li", "meng", "zong", \
	"zhi|ji", "nian", "hu", "yu", "di", "shi", "shen|can", "huan", "ti", "hou", \
	"xing|zheng", "zhu", "la", "zong", "ji|zei", "bian", "bian", "huan", "quan", "zei", \
	"wei", "wei", "yu", "chun", "rou", "die|qie", "huang", "lian", "yan", "qiu", \
	"qiu", "jian", "bi", "e", "yang", "fu", "sai|xi", "jian|gan", "xia", "wei|tuo", \
	"hu", "shi", "ruo", "xuan", "wen", "jian|qian", "hao", "wu", "fang|pang", "sao", \
	"liu", "ma", "shi", "shi", "guan|gun|kun", "zi", "teng", "die|ta", "yao", "ge|e", \
	"yong", "qian", "qi", "wen", "ruo", "shen", "lian", "ao", "le", "hui", \
	"min", "ji", "tiao", "qu", "jian", "shen|can", "man", "xi", "qiu", "biao", \
	"ji", "ji", "zhu", "jiang", "xiu|qiu", "zhuan|tuan", "yong", "zhang", "kang", "xue", \
	"bie", "yu", "qu", "xiang", "bo", "jiao", "xun", "su", "huang", "zun", \
	"shan|tuo", "shan", "fan", "jue|gui", "lin", "xun", "miao", "xi", "zeng", "xiang", \
	"fen", "guan", "hou", "kuai", "zei", "sao", "zhan|shan", "gan", "gui", "sheng|ying", \
	"li", "chang", "lei", "shu", "ai", "ru", "ji", "yu|xu", "hu", "shu", \
	"li", "la|lie", "li|lu", "mie", "zhen", "xiang", "e", "lu", "guan", "li", \
	"xian", "yu", "dao", "ji", "you", "tun", "lu|lv", "fang", "ba", "he|ge", \
	"ba|bo", "ping", "nian", "lu", "you", "zha", "chou|fu", "bo|ba", "bao|pao", "hou", \
	"ju|pi", "tai", "gui|xie", "jie|qia", "kao", "wei", "er", "tong", "zei", "hou", \
	"kuai", "ji", "jiao", "xian", "zha", "xiang|zhen", "xun", "geng", "li", "lian", \
	"jian", "li", "shi", "tiao", "gun", "sha", "huan", "jun", "ji|zei", "yong", \
	"qing|zheng", "ling", "qi", "zou", "fei", "kun", "chang", "gu", "ni", "nian", \
	"diao", "jing", "shen|can", "shi", "zi", "fen", "die|qie|zha", "bi", "chang", "ti", \
	"wen", "wei", "sai|xi", "e", "qiu", "fu", "huang", "quan", "jiang", "bian", \
	"sao", "ao", "qi", "die|ta", "guan|gun|kun", "yao", "fang|pang", "jian|qian", "le", "biao", \
	"xue", "bie", "man", "min", "yong", "wei", "xi", "jue|gui", "shan", "lin", \
	"zun", "hu", "gan", "li", "shan|zhan", "guan", "niao|diao", "yi", "fu", "li", \
	"jiu|qiu|zhi", "bu", "yan", "fu", "diao|zhao", "ji", "feng", "ru", "gan|han", "shi", \
	"feng", "ming", "bao", "yuan", "zhi|chi", "hu", "qin", "gui|fu", "fen|ban", "wen", \
	"qian|jian", "shi", "yu", "fou", "ao|yao", "jue|gui", "jue|gui", "pi", "huan", "zhen", \
	"bao", "yan", "ya", "zheng", "fang", "feng", "wen", "ou", "dai", "jia|ge", \
	"ru", "ling", "bi|mie", "fu", "tuo", "min|wen", "li", "bian", "zhi", "ge", \
	"yuan", "ci", "gou|qu", "xiao", "chi", "dan", "ju", "ao|yao", "gu", "zhong", \
	"yu", "yang", "yu", "ya", "die|tie|hu", "yu", "tian", "ying|xue", "dui", "wu", \
	"er", "gua", "ai", "zhi", "yan|e", "heng", "xiao", "jia", "lie", "zhu", \
	"yang|xiang", "yi|ti", "hong", "lu", "ru", "mou", "ge", "ren", "jiao|xiao", "xiu", \
	"zhou|diao", "chi", "luo|ge", "heng", "nian", "e", "luan", "jia", "ji", "tu", \
	"juan|huan", "tuo", "bu|pu", "wu", "juan", "yu", "bo", "jun", "jun", "bi", \
	"xi", "jun", "ju", "tu", "jing", "ti", "e", "e", "kuang", "hu|gu", \
	"wu", "shen", "lai|chi", "jiao", "pan", "lu", "pi", "shu", "fu", "an|ya", \
	"zhuo", "feng|peng", "qiu", "qian", "bei", "diao", "lu", "que", "jian", "ju", \
	"tu", "ya", "yuan", "qi", "li", "ye", "zhui", "kong", "duo", "kun", \
	"sheng", "qi", "jing", "yi", "yi", "qing|jing", "zi", "lai", "dong", "qi", \
	"chun|tuan", "geng", "ju", "qu|jue", "yi", "zun", "ji", "shu", "uu", "chi", \
	"miao", "rou", "an|ya", "qiu", "ti|chi", "hu", "ti|chi", "e", "jie", "mao", \
	"fu|bi", "chun", "tu", "yan", "he|jie", "yuan", "bian|pian", "kun", "mei", "hu", \
	"ying", "chuan|zhi", "wu", "ju", "dong", "qiang|cang", "fang", "hu|he", "ying", "yuan", \
	"xian", "weng", "shi", "he", "chu", "tang", "xia", "ruo", "liu", "ji", \
	"hu|gu", "jian|qian", "sun|xun", "han", "ci", "ci", "yi", "yao", "yan", "ji", \
	"li", "tian", "kou", "ti", "ti|si", "yi", "tu", "ma", "xiao", "gao", \
	"tian", "chen", "ji", "tuan", "zhe", "ao", "yao|xiao", "yi", "ou", "chi", \
	"zhi|zhe", "liu", "yong", "lv", "bi", "shuang", "zhuo", "yu", "wu", "jue", \
	"yin", "ti|tan", "si", "jiao", "yi", "hua", "bi", "ying", "su", "huang", \
	"fan", "jiao", "liao", "yan", "gao", "jiu", "xian", "xian", "tu", "mai", \
	"zun", "shu|yu", "ying", "lu", "tuan", "xian", "xue", "yi", "pi", "shu|zhu|chu", \
	"luo", "xi", "yi", "ji", "ze", "yu", "zhan", "ye", "yang", "bi|pi", \
	"ning", "hu", "mi", "ying", "mang|meng", "di", "yue", "yu", "lei", "bu", \
	"lu", "he", "long", "shuang", "yue", "ying", "huan|guan", "gou|qu", "li", "luan", \
	"niao|diao", "jiu|qiu|zhi", "ji", "yuan", "ming", "shi", "ou", "ya", "cang|qiang", "bao", \
	"zhen", "gu", "dong", "lu", "ya", "xiao", "yang", "ling", "chi", "gou|qu", \
	"yuan", "xue", "tuo", "si", "zhe|zhi", "er", "gua", "xiu", "heng", "zhou|diao", \
	"ge", "luan", "hong", "wu", "bo", "li", "juan", "hu|gu", "e", "yu", \
	"xian", "ti", "wu", "que", "miao", "an|ya", "kun", "bei", "feng|peng", "qian", \
	"chun|tuan", "geng", "yuan", "su", "hu", "he", "e", "hu|gu", "qiu", "ci", \
	"mei", "wu", "yi", "yao", "weng", "liu", "ji", "yi", "jian|qian", "he", \
	"yi", "ying", "zhe", "liu", "liao", "jiao", "jiu", "shu|yu", "lu", "huan", \
	"zhan", "ying", "hu", "mang|meng", "guan|huan|quan", "shuang", "lu", "jin", "ling", "jian", \
	"xian|jian", "cuo", "jian", "jian", "yan", "cuo", "lu|lv", "you", "cu", "ji", \
	"biao|pao", "cu", "pao", "cu|zhu", "jun|qun", "zhu", "jian", "mi", "mi", "yu", \
	"liu", "chen", "jun|qun", "lin", "ni", "qi", "lu", "jiu", "jun|qun", "jing", \
	"li|si", "xiang", "xian|yan", "jia", "mi", "li", "she", "zhang", "lin", "jing", \
	"qi", "ling", "yan", "cu", "mai", "mai", "he", "chao", "fu", "mian", \
	"mian", "fu", "pao", "qu", "qu", "mou", "fu", "xian|yan", "lai", "qu", \
	"mian", "chi", "feng", "fu", "qu", "mian", "ma", "me|mo", "me|ma|mo", "hui", \
	"mo", "zou", "nun", "fen", "huang", "huang", "jin", "guang", "tian", "tou", \
	"hong", "hua", "kuang", "hong", "shu", "li", "nian", "chi", "hei", "hei", \
	"yi", "qian", "dan", "xi", "tun", "mo", "mo", "jian|qian", "dai", "chu", \
	"yi|you", "dian|zhan", "yi", "xia", "yan", "qu", "mei", "yan", "qing", "yue|ye", \
	"lai|li", "dang|tang", "du", "can", "yan", "jian|yan", "yan", "zhen|dan", "an", "zhen|yan", \
	"dai|zhun", "can", "wa|yi", "mei", "dan|zhan", "yan", "du", "lu", "xian|zhi", "fen", \
	"fu", "fu", "mian|min|meng", "mian|min|meng", "yuan", "cu", "qu", "zhao|chao", "wa", "zhu", \
	"zhi", "meng", "ao", "bie", "tuo", "bi", "yuan", "zhao|chao", "tuo", "ding|zhen", \
	"mi|jiong", "nai", "ding|zhen", "zi", "gu", "gu", "dong", "fen", "tao", "yuan", \
	"pi", "chang", "gao", "cao|qi", "yuan", "tang", "teng", "shu", "shu", "fen", \
	"fei", "wen", "ba|fei", "diao", "tuo", "zhong", "qu", "sheng", "shi", "you", \
	"shi", "ting", "wu", "xi|ju", "jing", "hun", "xi|ju", "yan", "tu", "si", \
	"xi", "xian", "yan", "lei", "bi", "ya|yao", "qiu", "han", "wu|hui", "wu|hui", \
	"hou|ku", "xie", "he|e", "zha", "xiu", "weng", "zha", "nong", "nang", "qi|ji", \
	"zhai", "ji", "ji|zi", "ji", "ji", "ji|qi|zi", "ji", "chi", "chen", "chen", \
	"he", "ya", "yin|yan", "xie", "bao", "ze", "shi|xie", "zi|chai", "chi", "yan", \
	"ju|zha", "tiao", "ling", "ling", "chu", "quan", "shi|xie", "yin|ken", "nie", "jiu", \
	"yao", "chuo", "yun", "yu|wu", "chu", "qi|yi", "ni", "ce|ze|zha", "chuo|zou", "qu", \
	"yun", "yan", "yu|ou", "e", "wo", "yi", "ci|cuo", "zou", "dian", "chu", \
	"jin", "ya|e", "chi", "chen", "he", "yin|yan|ken", "ju|zha", "ling", "bao", "tiao", \
	"chai|zi", "yin|ken", "wu|yu", "chuo", "qu", "wo", "long|mang", "pang", "gong|wo", "long|pang", \
	"yan", "long|mang", "long", "gong", "kan|ke", "da", "ling", "da", "long|mang", "gong", \
	"kan|ke", "gui|jun|qiu", "qiu", "bie", "gui|jun|qiu", "yue", "chui", "he", "jiao", "xie", \
	"yu" };

int is_utf8_string(char* utf)
{
	int length = strlen(utf);
	int check_sub = 0;
	int i = 0;

	if (length > HZ2PY_UTF8_CHECK_LENGTH)	
	{
		length = HZ2PY_UTF8_CHECK_LENGTH;
	}

	for (; i < length; i++)
	{
		if (check_sub == 0)
		{
			if ((utf[i] >> 7) == 0)			
			{
				continue;
			}
			else if ((utf[i] & 0xE0) == 0xC0)	
			{
				check_sub = 1;
			}
			else if ((utf[i] & 0xF0) == 0xE0)	
			{
				check_sub = 2;
			}
			else if ((utf[i] & 0xF8) == 0xF0)
			{
				check_sub = 3;
			}
			else if ((utf[i] & 0xFC) == 0xF8)	
			{
				check_sub = 4;
			}
			else if ((utf[i] & 0xFE) == 0xFC)	
			{
				check_sub = 5;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if ((utf[i] & 0xC0) != 0x80)
			{
				return 0;
			}
			check_sub--;
		}
	}
	return 1;
}


void pinyin_utf8(char* inbuf, char* outbuf, bool m_blnSimaple = false, bool polyphone_support = false, bool m_blnFirstBig = false, bool m_blnAllBiG = false, bool m_LetterEnd = false, bool m_unknowSkip = true, bool m_filterPunc = true)
{
	int inbuf_len = strlen(inbuf);
	char* _tmp;
	char* _tmp2;
	char py_tmp[HZ2PY_MAX_PINYIN_SIZE] = "";	
	char py_tmp2[HZ2PY_MAX_PINYIN_SIZE] = "";	
	int uni;
	int iOutbuf = 0;
	char sep = '\'';		

	for (int i = 0; i < inbuf_len; i++)
	{
		if ((unsigned char)inbuf[i] < 0x80) { 
			if (m_filterPunc && !(inbuf[i] >= 'a' && inbuf[i] <= 'z' || inbuf[i] >= 'A' && inbuf[i] <= 'Z')) {
				continue;
			}
			if (!safeAddToOutbuf(outbuf, iOutbuf, &inbuf[i], 1)) return;
			//if(m_LetterEnd) if(!safeAddToOutbuf(outbuf,iOutbuf,&sep,1)) return;  //英文字母也打分隔符
			continue;
		}
		else if ((inbuf[i] & 0xE0) == 0xC0) {	
			if (i + 1 >= inbuf_len) {
				return;
			}
			if (!m_unknowSkip) {
				if (!safeAddToOutbuf(outbuf, iOutbuf, &inbuf[i], 2)) return;
			}
			i++;
		}
		else if ((inbuf[i] & 0xF0) == 0xE0) {	
			if (i + 2 >= inbuf_len) {
				return;
			}
			uni = (((int)(inbuf[i] & 0x0F)) << 12)
				| (((int)(inbuf[i + 1] & 0x3F)) << 6)
				| (inbuf[i + 2] & 0x3F);
			if (uni > 19967 && uni < 40870)
			{
				memset(py_tmp, '\0', sizeof(char) * HZ2PY_MAX_PINYIN_SIZE);
				memset(py_tmp2, '\0', sizeof(char) * HZ2PY_MAX_PINYIN_SIZE);

				strcpy_s(py_tmp, HZ2PY_MAX_PINYIN_SIZE, _pinyin_table_[uni - 19968]);

				_tmp = py_tmp;
				_tmp2 = py_tmp2;

				if (m_blnSimaple)	//只支持首字母
				{
					*_tmp2 = *_tmp;
					_tmp++;
					_tmp2++;
					while (*_tmp != '\0')
					{
						if (*_tmp == '|' || *(_tmp - 1) == '|')
						{
							*_tmp2 = *_tmp;
							_tmp2++;
						}
						_tmp++;
					}
					_tmp2 = py_tmp2;
				}
				else
				{
					strcpy_s(py_tmp2, HZ2PY_MAX_PINYIN_SIZE, py_tmp);
				}

				if (m_blnAllBiG)	//全部大写
				{
					while (*_tmp2 != '\0')
					{
						if (*_tmp2 >= 'a' && *_tmp2 <= 'z')
						{
							*_tmp2 = *_tmp2 - 32;
						}
						_tmp2++;
					}
					_tmp2 = py_tmp2;
				}

				if (m_blnFirstBig) {	//首字母大写
					if (*_tmp2 >= 'a' && *_tmp2 <= 'z')
					{
						*_tmp2 = *_tmp2 - 32;
					}
				}

				if (!polyphone_support)	//不支持多音字
				{
					while (*_tmp2 != '\0')
					{
						if (*_tmp2 == '|')
						{
							*_tmp2 = '\0';
							break;
						}
						_tmp2++;
					}

					_tmp2 = py_tmp2;
				}

				if (!safeAddToOutbuf(outbuf, iOutbuf, py_tmp2, strlen(py_tmp2)))	return;
				if (m_LetterEnd) if (!safeAddToOutbuf(outbuf, iOutbuf, &sep, 1)) return;
				i = i + 2;
			}
			else if (!m_unknowSkip) {
				if (!safeAddToOutbuf(outbuf, iOutbuf, &inbuf[i], 3)) return;
				i = i + 2;
			}
		}
		else  if ((inbuf[i] & 0xF8) == 0xF0) {//四个字节
			if (i + 3 >= inbuf_len) {
				return;
			}
			if (!m_unknowSkip) {
				if (!safeAddToOutbuf(outbuf, iOutbuf, &inbuf[i], 4)) return;
			}
			i = i + 3;
		}
		else if ((inbuf[i] & 0xFC) == 0xF8) {	//五个字节
			if (i + 4 >= inbuf_len) {
				return;
			}
			if (!m_unknowSkip) {
				if (!safeAddToOutbuf(outbuf, iOutbuf, &inbuf[i], 5)) return;
			}
			i = i + 4;
		}
		else if ((inbuf[i] & 0xFE) == 0xFC) {	//六个字节
			if (i + 5 >= inbuf_len) {
				return;
			}
			if (!m_unknowSkip) {
				if (!safeAddToOutbuf(outbuf, iOutbuf, &inbuf[i], 6)) return;
			}
			i = i + 5;
		}
		else {
			if (!m_unknowSkip)
			{
				if (!safeAddToOutbuf(outbuf, iOutbuf, &inbuf[i], 1)) return;
			}
			i++;
			//break;
		}
	}
}


int findLetter(int nCode, char* strValue)
{
	switch (nCode)
	{
	case 6325:
	case 6436:
	case 7571:
	case 7925:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "A");
		break;
	case 6263:
	case 6440:
	case 7040:
	case 7208:
	case 7451:
	case 7733:
	case 7945:
	case 8616:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "AI");
		break;
	case 5847:
	case 5991:
	case 6278:
	case 6577:
	case 6654:
	case 7281:
	case 7907:
	case 8038:
	case 8786:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "AN");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ANG");
		break;
	case 5974:
	case 6254:
	case 6427:
	case 6514:
	case 6658:
	case 6959:
	case 7033:
	case 7081:
	case 7365:
	case 8190:
	case 8292:
	case 8643:
	case 8701:
	case 8773:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "AO");
		break;
	case 6056:
	case 6135:
	case 6517:
	case 7857:
	case 8446:
	case 8649:
	case 8741:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BA");
		break;
	case 6267:
	case 6334:
	case 7494:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BAI");
		break;
	case 5870:
	case 5964:
	case 7851:
	case 8103:
	case 8113:
	case 8418:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BAN");
		break;
	case 6182:
	case 6826:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BANG");
		break;
	case 6165:
	case 7063:
	case 7650:
	case 8017:
	case 8157:
	case 8532:
	case 8621:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BAO");
		break;
	case 5635:
	case 5873:
	case 5893:
	case 5993:
	case 6141:
	case 6703:
	case 7753:
	case 8039:
	case 8156:
	case 8645:
	case 8725:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BEI");
		break;
	case 5946:
	case 5948:
	case 7458:
	case 7928:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BEN");
		break;
	case 6452:
	case 7420:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BENG");
		break;
	case 5616:
	case 5734:
	case 6074:
	case 6109:
	case 6221:
	case 6333:
	case 6357:
	case 6589:
	case 6656:
	case 6725:
	case 6868:
	case 6908:
	case 6986:
	case 6994:
	case 7030:
	case 7052:
	case 7221:
	case 7815:
	case 7873:
	case 7985:
	case 8152:
	case 8357:
	case 8375:
	case 8387:
	case 8416:
	case 8437:
	case 8547:
	case 8734:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BI");
		break;
	case 5650:
	case 5945:
	case 6048:
	case 6677:
	case 6774:
	case 7134:
	case 7614:
	case 7652:
	case 7730:
	case 7760:
	case 8125:
	case 8159:
	case 8289:
	case 8354:
	case 8693:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BIAN");
		break;
	case 7027:
	case 7084:
	case 7609:
	case 7613:
	case 7958:
	case 7980:
	case 8106:
	case 8149:
	case 8707:
	case 8752:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BIAO");
		break;
	case 8531:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BIE");
		break;
	case 5747:
	case 6557:
	case 7145:
	case 7167:
	case 7336:
	case 7375:
	case 7587:
	case 7957:
	case 8738:
	case 8762:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BIN");
		break;
	case 5787:
	case 5891:
	case 6280:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BING");
		break;
	case 5781:
	case 6403:
	case 6636:
	case 7362:
	case 7502:
	case 7771:
	case 7864:
	case 8030:
	case 8404:
	case 8543:
	case 8559:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BO");
		break;
	case 6318:
	case 6945:
	case 7419:
	case 7446:
	case 7848:
	case 7863:
	case 8519:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BU");
		break;
	case 6474:
	case 7769:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CA");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CAI");
		break;
	case 6978:
	case 7078:
	case 7218:
	case 8451:
	case 8785:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CAN");
		break;
	case 5687:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CANG");
		break;
	case 6448:
	case 6878:
	case 8309:
	case 8429:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CAO");
		break;
	case 6692:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CE");
		break;
	case 6515:
	case 6825:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CEN");
		break;
	case 6465:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CENG");
		break;
	case 6639:
	case 6766:
	case 7017:
	case 7230:
	case 7311:
	case 7322:
	case 7363:
	case 7942:
	case 7979:
	case 8135:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHA");
		break;
	case 5713:
	case 7846:
	case 8091:
	case 8218:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHAI");
		break;
	case 5770:
	case 5838:
	case 6159:
	case 6667:
	case 6893:
	case 6904:
	case 6981:
	case 7031:
	case 7086:
	case 7472:
	case 7688:
	case 7966:
	case 8324:
	case 8580:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHAN");
		break;
	case 5686:
	case 5943:
	case 6041:
	case 6137:
	case 6660:
	case 6568:
	case 6749:
	case 7029:
	case 7047:
	case 7438:
	case 7509:
	case 8680:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHANG");
		break;
	case 6687:
	case 7443:
	case 8173:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHAO");
		break;
	case 5969:
	case 7726:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHE");
		break;
	case 5840:
	case 5863:
	case 6251:
	case 6433:
	case 6923:
	case 7201:
	case 7320:
	case 7755:
	case 8619:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHEN");
		break;
	case 5609:
	case 5984:
	case 7239:
	case 7263:
	case 7583:
	case 7810:
	case 7881:
	case 7905:
	case 8146:
	case 8241:
	case 8508:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHENG");
		break;
	case 5749:
	case 6015:
	case 6061:
	case 6319:
	case 6374:
	case 6420:
	case 6445:
	case 6633:
	case 7042:
	case 7523:
	case 7787:
	case 8023:
	case 8101:
	case 8161:
	case 8231:
	case 8304:
	case 8355:
	case 8388:
	case 8489:
	case 8556:
	case 8746:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHI");
		break;
	case 6091:
	case 6671:
	case 6731:
	case 8409:
	case 8430:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHONG");
		break;
	case 5717:
	case 6492:
	case 6716:
	case 8112:
	case 8637:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHOU");
		break;
	case 5601:
	case 5927:
	case 6680:
	case 6732:
	case 7109:
	case 7238:
	case 7290:
	case 7343:
	case 8150:
	case 8260:
	case 8573:
	case 8777:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHU");
		break;
	case 6285:
	case 6408:
	case 7590:
	case 8563:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUAI");
		break;
	case 6622:
	case 6955:
	case 7516:
	case 7843:
	case 8413:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUAN");
		break;
	case 6675:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUANG");
		break;
	case 5879:
	case 7302:
	case 7319:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUI");
		break;
	case 6127:
	case 8040:
	case 8277:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUN");
		break;
	case 7401:
	case 8554:
	case 8626:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUO");
		break;
	case 6075:
	case 6358:
	case 7684:
	case 8043:
	case 8457:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CI");
		break;
	case 6042:
	case 6840:
	case 7085:
	case 7193:
	case 7214:
	case 7240:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CONG");
		break;
	case 7308:
	case 7403:
	case 7577:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "COU");
		break;
	case 6180:
	case 6562:
	case 6607:
	case 7367:
	case 8501:
	case 8530:
	case 8577:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CU");
		break;
	case 5764:
	case 6305:
	case 7664:
	case 7973:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CUAN");
		break;
	case 6718:
	case 6145:
	case 6393:
	case 7213:
	case 7333:
	case 7505:
	case 8631:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CUI");
		break;
	case 6666:
	case 8169:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CUN");
		break;
	case 5640:
	case 6547:
	case 7566:
	case 7917:
	case 7983:
	case 8078:
	case 8526:
	case 8567:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CUO");
		break;
	case 6239:
	case 6353:
	case 6410:
	case 6682:
	case 7007:
	case 8155:
	case 8346:
	case 8716:
	case 8718:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DA");
		break;
	case 6004:
	case 6316:
	case 6523:
	case 6942:
	case 7110:
	case 7173:
	case 8776:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DAI");
		break;
	case 5757:
	case 6144:
	case 6402:
	case 7373:
	case 7470:
	case 7781:
	case 8067:
	case 8087:
	case 8185:
	case 8376:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DAN");
		break;
	case 5852:
	case 5942:
	case 6148:
	case 6920:
	case 7724:
	case 7885:
	case 8141:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DANG");
		break;
	case 6322:
	case 6665:
	case 7514:
	case 8478:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DAO");
		break;
	case 7929:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DE");
		break;
	case 6466:
	case 6556:
	case 7413:
	case 7767:
	case 7975:
	case 8403:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DENG");
		break;
	case 5621:
	case 5765:
	case 5814:
	case 5848:
	case 5901:
	case 5970:
	case 6122:
	case 6454:
	case 7023:
	case 7116:
	case 7260:
	case 7306:
	case 7475:
	case 7738:
	case 7758:
	case 7791:
	case 7965:
	case 8438:
	case 8730:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DI");
		break;
	case 6439:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DIA");
		break;
	case 5871:
	case 5967:
	case 6559:
	case 7172:
	case 7868:
	case 8116:
	case 8118:
	case 8401:
	case 8558:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DIAN");
		break;
	case 7886:
	case 8585:
	case 8684:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DIAO");
		break;
	case 5976:
	case 6006:
	case 6273:
	case 6409:
	case 7526:
	case 8012:
	case 8183:
	case 8562:
	case 8688:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DIE");
		break;
	case 5674:
	case 6404:
	case 7164:
	case 7575:
	case 7754:
	case 7814:
	case 8059:
	case 8184:
	case 8490:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DING");
		break;
	case 7891:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DIU");
		break;
	case 5977:
	case 6343:
	case 6520:
	case 6528:
	case 7517:
	case 7543:
	case 7556:
	case 7747:
	case 8020:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DONG");
		break;
	case 6190:
	case 8128:
	case 8229:
	case 8391:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DOU");
		break;
	case 6022:
	case 6429:
	case 6834:
	case 7292:
	case 7525:
	case 8328:
	case 8338:
	case 8739:
	case 8782:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DU");
		break;
	case 7318:
	case 7649:
	case 8393:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DUAN");
		break;
	case 7701:
	case 7713:
	case 7752:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DUI");
		break;
	case 6771:
	case 7632:
	case 7727:
	case 7766:
	case 7779:
	case 7970:
	case 8527:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DUN");
		break;
	case 6345:
	case 6365:
	case 6785:
	case 7122:
	case 7876:
	case 8154:
	case 8566:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DUO");
		break;
	case 5612:
	case 5832:
	case 5844:
	case 5949:
	case 6035:
	case 6113:
	case 6164:
	case 6332:
	case 6721:
	case 6977:
	case 7025:
	case 7378:
	case 7581:
	case 7916:
	case 7941:
	case 8042:
	case 8206:
	case 8689:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "E");
		break;
	case 6176:
	case 6284:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "EN");
		break;
	case 5706:
	case 6939:
	case 7177:
	case 7879:
	case 8025:
	case 8660:
	case 2294:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ER");
		break;
	case 5950:
	case 7732:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FA");
		break;
	case 6212:
	case 6232:
	case 6506:
	case 7283:
	case 7660:
	case 7818:
	case 8576:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FAN");
		break;
	case 5890:
	case 7242:
	case 7853:
	case 8419:
	case 8648:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FANG");
		break;
	case 6032:
	case 6584:
	case 6713:
	case 6839:
	case 6990:
	case 7119:
	case 7328:
	case 7572:
	case 7619:
	case 7673:
	case 7948:
	case 8082:
	case 8267:
	case 8385:
	case 8468:
	case 8613:
	case 8678:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FEI");
		break;
	case 5739:
	case 6915:
	case 7291:
	case 8687:
	case 8787:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FEN");
		break;
	case 5726:
	case 5926:
	case 6155:
	case 6384:
	case 6767:
	case 7731:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FENG");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FO");
		break;
	case 8330:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FOU");
		break;
	case 5775:
	case 5776:
	case 5914:
	case 6029:
	case 6062:
	case 6119:
	case 6142:
	case 6252:
	case 6327:
	case 6505:
	case 6686:
	case 6870:
	case 6985:
	case 7058:
	case 7066:
	case 7106:
	case 7108:
	case 7285:
	case 7471:
	case 7680:
	case 7741:
	case 7774:
	case 7775:
	case 7823:
	case 7991:
	case 8005:
	case 8222:
	case 8261:
	case 8280:
	case 8283:
	case 8479:
	case 8535:
	case 8538:
	case 8654:
	case 8691:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FU");
		break;
	case 6246:
	case 7056:
	case 7057:
	case 7424:
	case 7837:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GA");
		break;
	case 5604:
	case 5875:
	case 5982:
	case 7414:
	case 7464:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GAI");
		break;
	case 5965:
	case 6053:
	case 6247:
	case 6306:
	case 6779:
	case 6838:
	case 6887:
	case 7104:
	case 7347:
	case 7426:
	case 7723:
	case 8065:
	case 8491:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GAN");
		break;
	case 7716:
	case 7824:
	case 8364:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GANG");
		break;
	case 5626:
	case 5830:
	case 5912:
	case 6227:
	case 7141:
	case 7332:
	case 7334:
	case 7429:
	case 7915:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GAO");
		break;
	case 5610:
	case 5678:
	case 5933:
	case 5957:
	case 6010:
	case 6435:
	case 7092:
	case 7501:
	case 7585:
	case 7749:
	case 7951:
	case 8143:
	case 8220:
	case 8420:
	case 8732:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GE");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GEI");
		break;
	case 5608:
	case 6102:
	case 6371:
	case 8462:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GEN");
		break;
	case 6376:
	case 6657:
	case 7114:
	case 8665:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GENG");
		break;
	case 7178:
	case 7537:
	case 8228:
	case 8601:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GONG");
		break;
	case 5694:
	case 5824:
	case 6524:
	case 6960:
	case 7037:
	case 7135:
	case 7259:
	case 7477:
	case 7616:
	case 8349:
	case 8384:
	case 8724:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GOU");
		break;
	case 5637:
	case 5812:
	case 6152:
	case 6536:
	case 6773:
	case 7284:
	case 7379:
	case 7484:
	case 7486:
	case 7591:
	case 7617:
	case 7813:
	case 7825:
	case 7860:
	case 7932:
	case 8019:
	case 8083:
	case 8233:
	case 8494:
	case 8593:
	case 8681:
	case 8729:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GU");
		break;
	case 5652:
	case 5820:
	case 6341:
	case 7273:
	case 7550:
	case 8027:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUA");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUAI");
		break;
	case 5736:
	case 6124:
	case 6272:
	case 6842:
	case 7834:
	case 8057:
	case 8170:
	case 8704:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUAN");
		break;
	case 6359:
	case 6578:
	case 7270:
	case 7555:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUANG");
		break;
	case 5648:
	case 5659:
	case 6649:
	case 7003:
	case 7277:
	case 7433:
	case 7448:
	case 8007:
	case 8394:
	case 8657:
	case 8712:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUI");
		break;
	case 5782:
	case 7121:
	case 7762:
	case 8671:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUN");
		break;
	case 5769:
	case 6266:
	case 6335:
	case 6494:
	case 6538:
	case 6603:
	case 7304:
	case 7529:
	case 8188:
	case 8268:
	case 8269:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUO");
		break;
	case 7894:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HA");
		break;
	case 6443:
	case 7560:
	case 8516:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HAI");
		break;
	case 5885:
	case 6153:
	case 6294:
	case 6759:
	case 6911:
	case 7447:
	case 7642:
	case 8192:
	case 8205:
	case 8232:
	case 8793:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HAN");
		break;
	case 6776:
	case 7112:
	case 8194:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HANG");
		break;
	case 6179:
	case 6222:
	case 6438:
	case 6467:
	case 6909:
	case 6916:
	case 7427:
	case 8009:
	case 8211:
	case 8226:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HAO");
		break;
	case 5813:
	case 5932:
	case 5954:
	case 6432:
	case 6756:
	case 7434:
	case 7833:
	case 8202:
	case 8234:
	case 8471:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HE");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HEI");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HEN");
		break;
	case 6231:
	case 7181:
	case 7276:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HENG");
		break;
	case 5768:
	case 5774:
	case 5807:
	case 6106:
	case 6214:
	case 6216:
	case 6740:
	case 6792:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HONG");
		break;
	case 6009:
	case 6565:
	case 6943:
	case 8090:
	case 8383:
	case 8455:
	case 8655:
	case 8731:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HOU");
		break;
	case 5792:
	case 6392:
	case 6481:
	case 6518:
	case 6609:
	case 6679:
	case 6717:
	case 6816:
	case 6879:
	case 7190:
	case 7346:
	case 7385:
	case 7618:
	case 7635:
	case 7646:
	case 7670:
	case 7672:
	case 7679:
	case 8013:
	case 8032:
	case 8041:
	case 8055:
	case 8343:
	case 8513:
	case 8590:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HU");
		break;
	case 7072:
	case 7275:
	case 7725:
	case 7892:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUA");
		break;
	case 8555:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUAI");
		break;
	case 5928:
	case 6140:
	case 6307:
	case 6487:
	case 6621:
	case 6801:
	case 6829:
	case 6881:
	case 6930:
	case 6953:
	case 7157:
	case 7944:
	case 8673:
	case 8763:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUAN");
		break;
	case 5882:
	case 6569:
	case 6850:
	case 6874:
	case 6956:
	case 7211:
	case 7533:
	case 8105:
	case 8308:
	case 8382:
	case 8692:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUANG");
		break;
	case 5822:
	case 6078:
	case 6086:
	case 6205:
	case 6352:
	case 6360:
	case 6425:
	case 6736:
	case 6807:
	case 6811:
	case 6971:
	case 7132:
	case 7185:
	case 7445:
	case 7703:
	case 8219:
	case 8319:
	case 8766:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUI");
		break;
	case 5827:
	case 6638:
	case 6752:
	case 6867:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUN");
		break;
	case 5669:
	case 6229:
	case 6311:
	case 6475:
	case 6623:
	case 7856:
	case 7933:
	case 7976:
	case 8175:
	case 8322:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUO");
		break;
	case 5629:
	case 5632:
	case 5662:
	case 5705:
	case 5742:
	case 5952:
	case 6024:
	case 6033:
	case 6193:
	case 6210:
	case 6265:
	case 6320:
	case 6350:
	case 6383:
	case 6507:
	case 6553:
	case 6809:
	case 6976:
	case 7087:
	case 7160:
	case 7165:
	case 7314:
	case 7374:
	case 7410:
	case 7411:
	case 7469:
	case 7473:
	case 7487:
	case 7620:
	case 7722:
	case 7831:
	case 7990:
	case 8002:
	case 8104:
	case 8217:
	case 8337:
	case 8339:
	case 8463:
	case 8550:
	case 8611:
	case 8661:
	case 8674:
	case 8757:
	case 8768:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JI");
		break;
	case 5704:
	case 5903:
	case 6171:
	case 6521:
	case 6804:
	case 6940:
	case 7176:
	case 7409:
	case 7546:
	case 7702:
	case 7882:
	case 7956:
	case 8072:
	case 8142:
	case 8244:
	case 8353:
	case 8434:
	case 8542:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIA");
		break;
	case 5752:
	case 5841:
	case 5857:
	case 6149:
	case 6183:
	case 6286:
	case 6853:
	case 6931:
	case 6932:
	case 7144:
	case 7237:
	case 7305:
	case 7407:
	case 7415:
	case 7480:
	case 7489:
	case 7506:
	case 7576:
	case 7790:
	case 7921:
	case 8047:
	case 8148:
	case 8340:
	case 8469:
	case 8534:
	case 8561:
	case 8668:
	case 8721:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIAN");
		break;
	case 6092:
	case 6814:
	case 7113:
	case 7154:
	case 7481:
	case 7768:
	case 8180:
	case 8461:
	case 8488:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIANG");
		break;
	case 5714:
	case 5753:
	case 6020:
	case 6090:
	case 6256:
	case 6461:
	case 6572:
	case 7015:
	case 7524:
	case 8008:
	case 8052:
	case 8252:
	case 8520:
	case 8551:
	case 8662:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIAO");
		break;
	case 5806:
	case 5821:
	case 6255:
	case 6414:
	case 7028:
	case 7061:
	case 7278:
	case 7757:
	case 8060:
	case 8201:
	case 8227:
	case 8441:
	case 8658:
	case 8726:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIE");
		break;
	case 5865:
	case 6103:
	case 6132:
	case 6468:
	case 6643:
	case 6659:
	case 7138:
	case 7210:
	case 7340:
	case 7465:
	case 7478:
	case 8138:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIN");
		break;
	case 5751:
	case 5869:
	case 6128:
	case 6616:
	case 6729:
	case 6794:
	case 6941:
	case 6982:
	case 7026:
	case 7534:
	case 7554:
	case 7570:
	case 7626:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIANG");
		break;
	case 6936:
	case 7671:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIONG");
		break;
	case 5754:
	case 6417:
	case 6746:
	case 7249:
	case 7274:
	case 8015:
	case 8053:
	case 8481:
	case 8761:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIU");
		break;
	case 5738:
	case 5810:
	case 6036:
	case 6058:
	case 6076:
	case 6268:
	case 6965:
	case 6980:
	case 7202:
	case 7307:
	case 7316:
	case 7323:
	case 7357:
	case 7381:
	case 7488:
	case 7611:
	case 7850:
	case 7924:
	case 8022:
	case 8132:
	case 8153:
	case 8482:
	case 8522:
	case 8565:
	case 8620:
	case 8634:
	case 8722:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JU");
		break;
	case 5918:
	case 6590:
	case 6824:
	case 7280:
	case 7835:
	case 7935:
	case 7952:
	case 8633:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JUAN");
		break;
	case 5642:
	case 5667:
	case 5860:
	case 5939:
	case 6207:
	case 6421:
	case 6457:
	case 6469:
	case 6540:
	case 6617:
	case 7062:
	case 7169:
	case 7286:
	case 7351:
	case 7663:
	case 7967:
	case 8574:
	case 8591:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JUE");
		break;
	case 6260:
	case 8168:
	case 8362:
	case 8769:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JUN");
		break;
	case 5671:
	case 6339:
	case 7544:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KA");
		break;
	case 5660:
	case 5978:
	case 6160:
	case 6673:
	case 6693:
	case 7888:
	case 7920:
	case 7939:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KAI");
		break;
	case 5709:
	case 6108:
	case 7412:
	case 7772:
	case 7811:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KAN");
		break;
	case 5688:
	case 6742:
	case 7854:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KANG");
		break;
	case 6974:
	case 7264:
	case 7491:
	case 7877:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KAO");
		break;
	case 6430:
	case 6519:
	case 6701:
	case 6859:
	case 7076:
	case 7128:
	case 7170:
	case 7380:
	case 7520:
	case 7807:
	case 7861:
	case 7930:
	case 7993:
	case 8066:
	case 8129:
	case 8204:
	case 8282:
	case 8733:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KE");
		break;
	case 8144:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KEN");
		break;
	case 7912:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KENG");
		break;
	case 5737:
	case 6539:
	case 8377:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KONG");
		break;
	case 6050:
	case 6202:
	case 6321:
	case 7778:
	case 8356:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KOU");
		break;
	case 5658:
	case 6005:
	case 6423:
	case 7111:
	case 8728:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KU");
		break;
	case 5708:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUA");
		break;
	case 5665:
	case 5906:
	case 6364:
	case 6586:
	case 7558:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUAI");
		break;
	case 8737:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUAN");
		break;
	case 5818:
	case 5831:
	case 5887:
	case 5959:
	case 6237:
	case 6349:
	case 7094:
	case 7460:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUANG");
		break;
	case 5624:
	case 5649:
	case 5771:
	case 6162:
	case 6281:
	case 6413:
	case 6416:
	case 6720:
	case 6951:
	case 7450:
	case 7805:
	case 8606:
	case 8743:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUI");
		break;
	case 6204:
	case 6245:
	case 6458:
	case 6618:
	case 6928:
	case 7152:
	case 7841:
	case 8051:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIAO");
		break;
	case 5793:
	case 5988:
	case 6270:
	case 6354:
	case 6803:
	case 8483:
	case 8581:
	case 8764:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIE");
		break;
	case 6194:
	case 6388:
	case 6555:
	case 6662:
	case 6733:
	case 6964:
	case 7361:
	case 7405:
	case 7602:
	case 7812:
	case 8452:
	case 8579:
	case 8775:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIN");
		break;
	case 5925:
	case 6063:
	case 6342:
	case 6482:
	case 6786:
	case 7117:
	case 7258:
	case 7289:
	case 7418:
	case 8186:
	case 8240:
	case 8465:
	case 8676:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LING");
		break;
	case 6815:
	case 6962:
	case 7082:
	case 7124:
	case 7628:
	case 7654:
	case 7919:
	case 7954:
	case 8050:
	case 8644:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIU");
		break;
	case 5966:
	case 6055:
	case 6781:
	case 7171:
	case 7248:
	case 7542:
	case 7735:
	case 8110:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LONG");
		break;
	case 5745:
	case 6168:
	case 6422:
	case 6548:
	case 7946:
	case 8092:
	case 8179:
	case 8287:
	case 8735:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LOU");
		break;
	case 6744:
	case 7321:
	case 7586:
	case 7918:
	case 7989:
	case 8158:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LV");
		break;
	case 5968:
	case 6303:
	case 6464:
	case 6782:
	case 6843:
	case 6885:
	case 6954:
	case 7220:
	case 7251:
	case 7354:
	case 7391:
	case 7404:
	case 7510:
	case 7545:
	case 7969:
	case 8021:
	case 8056:
	case 8392:
	case 8421:
	case 8652:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LU");
		break;
	case 5785:
	case 7014:
	case 7279:
	case 8029:
	case 8639:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LUAN");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LUE");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LUN");
		break;
	case 5732:
	case 5789:
	case 6093:
	case 6259:
	case 6291:
	case 6604:
	case 6788:
	case 6880:
	case 7183:
	case 7301:
	case 7565:
	case 7961:
	case 8107:
	case 8635:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LUO");
		break;
	case 6328:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "M");
		break;
	case 6373:
	case 6579:
	case 7054:
	case 7231:
	case 8301:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MA");
		break;
	case 5929:
	case 6104:
	case 8618:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MAI");
		break;
	case 6012:
	case 6503:
	case 7147:
	case 7655:
	case 7960:
	case 8209:
	case 8293:
	case 8709:
	case 8720:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MAN");
		break;
	case 5888:
	case 6861:
	case 7743:
	case 8294:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MANG");
		break;
	case 5783:
	case 6066:
	case 6525:
	case 6787:
	case 7203:
	case 7436:
	case 7483:
	case 7503:
	case 7624:
	case 7714:
	case 7806:
	case 8317:
	case 8754:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MAO");
		break;
	case 6114:
	case 6550:
	case 6613:
	case 6828:
	case 6856:
	case 7325:
	case 7949:
	case 8044:
	case 8139:
	case 8740:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MEI");
		break;
	case 6249:
	case 7643:
	case 7715:
	case 7845:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MEN");
		break;
	case 5934:
	case 6189:
	case 6211:
	case 6734:
	case 7592:
	case 7770:
	case 8221:
	case 8276:
	case 8323:
	case 8427:
	case 8431:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MENG");
		break;
	case 5634:
	case 5855:
	case 6234:
	case 6368:
	case 6455:
	case 6608:
	case 6772:
	case 6921:
	case 6984:
	case 7563:
	case 7682:
	case 8445:
	case 8767:
	case 8771:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MI");
		break;
	case 6770:
	case 6837:
	case 6847:
	case 7579:
	case 7777:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIAN");
		break;
	case 6387:
	case 6967:
	case 7131:
	case 7149:
	case 7234:
	case 7721:
	case 7780:
	case 8037:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIAO");
		break;
	case 5631:
	case 6367:
	case 8326:
	case 8390:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIE");
		break;
	case 6069:
	case 6526:
	case 6741:
	case 6793:
	case 7137:
	case 7168:
	case 7175:
	case 7710:
	case 8710:
	case 8628:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIN");
		break;
	case 5804:
	case 6088:
	case 6873:
	case 7452:
	case 7808:
	case 8504:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MING");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIU");
		break;
	case 5851:
	case 6052:
	case 6175:
	case 6641:
	case 7038:
	case 7366:
	case 7950:
	case 7987:
	case 8102:
	case 8182:
	case 8586:
	case 8588:
	case 8765:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MO");
		break;
	case 5716:
	case 6372:
	case 7788:
	case 8254:
	case 8290:
	case 8642:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MOU");
		break;
	case 5679:
	case 5973:
	case 6057:
	case 6769:
	case 7504:
	case 7866:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MU");
		break;
	case 6437:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "N");
		break;
	case 6264:
	case 7539:
	case 7953:
	case 8136:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NA");
		break;
	case 5630:
	case 6021:
	case 6133:
	case 7245:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NAI");
		break;
	case 6411:
	case 6478:
	case 6479:
	case 7310:
	case 7578:
	case 8279:
	case 8486:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NAN");
		break;
	case 6313:
	case 6476:
	case 6646:
	case 7457:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NANG");
		break;
	case 5611:
	case 5981:
	case 6346:
	case 6614:
	case 7207:
	case 7748:
	case 7883:
	case 8245:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NAO");
		break;
	case 5811:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NE");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NEI");
		break;
	case 7705:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NEN");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NENG");
		break;
	case 5703:
	case 5972:
	case 6605:
	case 6685:
	case 7439:
	case 7627:
	case 7711:
	case 7794:
	case 7874:
	case 8682:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NI");
		break;
	case 5605:
	case 5994:
	case 7393:
	case 8004:
	case 8651:
	case 8683:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIAN");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIANG");
		break;
	case 6064:
	case 7053:
	case 7569:
	case 8433:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIAO");
		break;
	case 5877:
	case 6233:
	case 6431:
	case 8208:
	case 8411:
	case 8570:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIE");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIN");
		break;
	case 5690:
	case 6344:
	case 6924:
	case 8187:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NING");
		break;
	case 6580:
	case 6678:
	case 7004:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIU");
		break;
	case 5715:
	case 6370:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NONG");
		break;
	case 8181:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NOU");
		break;
	case 6983:
	case 7032:
	case 7059:
	case 7069:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NU");
		break;
	case 7704:
	case 7847:
	case 8412:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NV");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NUAN");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NUE");
		break;
	case 5748:
	case 6289:
	case 6386:
	case 7927:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NUO");
		break;
	case 6424:
	case 6462:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "O");
		break;
	case 5809:
	case 6670:
	case 7417:
	case 8178:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "OU");
		break;
	case 6166:
	case 7243:
	case 8365:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PA");
		break;
	case 5729:
	case 6169:
	case 6363:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PAI");
		break;
	case 6761:
	case 6790:
	case 8140:
	case 8165:
	case 8320:
	case 8571:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PAN");
		break;
	case 6561:
	case 6872:
	case 6944:
	case 8306:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PANG");
		break;
	case 6243:
	case 6583:
	case 6650:
	case 7567:
	case 8069:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PAO");
		break;
	case 6446:
	case 6490:
	case 7623:
	case 7934:
	case 8512:
	case 8612:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PEI");
		break;
	case 6852:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PEN");
		break;
	case 6001:
	case 6456:
	case 6681:
	case 8318:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PENG");
		break;
	case 5607:
	case 5682:
	case 5880:
	case 5892:
	case 5915:
	case 5960:
	case 6017:
	case 6037:
	case 6308:
	case 6472:
	case 6647:
	case 6836:
	case 7039:
	case 7102:
	case 7233:
	case 7422:
	case 7802:
	case 7828:
	case 7875:
	case 8117:
	case 8166:
	case 8223:
	case 8271:
	case 8589:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PI");
		break;
	case 5850:
	case 7073:
	case 7490:
	case 7561:
	case 8470:
	case 8568:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PIAN");
		break;
	case 5666:
	case 6449:
	case 7046:
	case 7146:
	case 7372:
	case 7809:
	case 8310:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PIAO");
		break;
	case 6054:
	case 7513:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PIE");
		break;
	case 7041:
	case 6253:
	case 7016:
	case 7315:
	case 7482:
	case 8213:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PIN");
		break;
	case 5723:
	case 7019:
	case 7250:
	case 8650:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PING");
		break;
	case 5647:
	case 5922:
	case 7174:
	case 7839:
	case 7862:
	case 8011:
	case 8345:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PO");
		break;
	case 5786:
	case 6269:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "POU");
		break;
	case 5773:
	case 6459:
	case 6863:
	case 6907:
	case 7217:
	case 7511:
	case 7968:
	case 7972:
	case 8575:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PU");
		break;
	case 5633:
	case 5725:
	case 5963:
	case 6027:
	case 6046:
	case 6089:
	case 6129:
	case 6134:
	case 6161:
	case 6213:
	case 6366:
	case 6450:
	case 6508:
	case 6510:
	case 6764:
	case 6831:
	case 7075:
	case 7118:
	case 7187:
	case 7189:
	case 7229:
	case 7271:
	case 7342:
	case 7440:
	case 7605:
	case 7687:
	case 7712:
	case 7751:
	case 8193:
	case 8251:
	case 8264:
	case 8475:
	case 8476:
	case 8572:
	case 8702:
	case 8772:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QI");
		break;
	case 6154:
	case 8736:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIA");
		break;
	case 5727:
	case 5761:
	case 5868:
	case 6023:
	case 6045:
	case 6071:
	case 6271:
	case 6509:
	case 6705:
	case 6727:
	case 6925:
	case 6926:
	case 6929:
	case 7155:
	case 7293:
	case 7541:
	case 7709:
	case 7852:
	case 8215:
	case 8373:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIAN");
		break;
	case 6762:
	case 7045:
	case 7341:
	case 7408:
	case 7633:
	case 7926:
	case 7947:
	case 7974:
	case 8163:
	case 8262:
	case 8439:
	case 8536:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIANG");
		break;
	case 5668:
	case 5829:
	case 5859:
	case 6081:
	case 6529:
	case 6724:
	case 6730:
	case 7352:
	case 7745:
	case 8546:
	case 8719:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIAO");
		break;
	case 5907:
	case 6711:
	case 7010:
	case 7492:
	case 7938:
	case 8370:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIE");
		break;
	case 6043:
	case 6276:
	case 6336:
	case 6426:
	case 6463:
	case 6858:
	case 7353:
	case 7923:
	case 8291:
	case 8432:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIN");
		break;
	case 6060:
	case 6485:
	case 7349:
	case 7764:
	case 8263:
	case 8332:
	case 8368:
	case 8605:
	case 8675:
	case 8784:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QING");
		break;
	case 5886:
	case 6068:
	case 8123:
	case 8243:
	case 8344:
	case 8528:
	case 8638:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIONG");
		break;
	case 5720:
	case 5947:
	case 6576:
	case 6848:
	case 6947:
	case 6957:
	case 7317:
	case 7468:
	case 8216:
	case 8239:
	case 8288:
	case 8435:
	case 8460:
	case 8690:
	case 8792:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIU");
		break;
	case 5816:
	case 5930:
	case 6201:
	case 6230:
	case 6511:
	case 6573:
	case 6754:
	case 7219:
	case 7479:
	case 7512:
	case 7552:
	case 7678:
	case 7765:
	case 8119:
	case 8248:
	case 8329:
	case 8480:
	case 8636:
	case 8781:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QU");
		break;
	case 5825:
	case 6085:
	case 6710:
	case 7125:
	case 7390:
	case 7816:
	case 7893:
	case 8273:
	case 8360:
	case 8760:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QUAN");
		break;
	case 6755:
	case 6758:
	case 7708:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QUE");
		break;
	case 6950:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QUN");
		break;
	case 6059:
	case 8237:
	case 8755:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RAN");
		break;
	case 7692:
	case 8006:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RANG");
		break;
	case 6073:
	case 7012:
	case 7267:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RAO");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RE");
		break;
	case 5680:
	case 6083:
	case 6156:
	case 6631:
	case 7377:
	case 7994:
	case 8137:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "REN");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RENG");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RI");
		break;
	case 6541:
	case 6585:
	case 7337:
	case 7532:
	case 8278:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RONG");
		break;
	case 8459:
	case 8569:
	case 8723:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ROU");
		break;
	case 6174:
	case 6224:
	case 6473:
	case 6818:
	case 6865:
	case 6906:
	case 7140:
	case 7908:
	case 8164:
	case 8212:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RU");
		break;
	case 7535:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RUAN");
		break;
	case 6039:
	case 6208:
	case 7236:
	case 7803:
	case 8224:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RUI");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RUN");
		break;
	case 5728:
	case 8372:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RUO");
		break;
	case 5606:
	case 5677:
	case 7493:
	case 7559:
	case 7610:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SA");
		break;
	case 6471:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SAI");
		break;
	case 6644:
	case 7507:
	case 8454:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SAN");
		break;
	case 6290:
	case 7763:
	case 8210:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SANG");
		break;
	case 6003:
	case 7150:
	case 7156:
	case 7593:
	case 8094:
	case 8694:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SAO");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SE");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SEN");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SENG");
		break;
	case 6394:
	case 7606:
	case 7901:
	case 8080:
	case 8436:
	case 8614:
	case 8672:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHA");
		break;
	case 8507:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHAI");
		break;
	case 5663:
	case 5808:
	case 5923:
	case 5979:
	case 6047:
	case 6890:
	case 7009:
	case 7051:
	case 7083:
	case 7594:
	case 7844:
	case 8062:
	case 8321:
	case 8414:
	case 8539:
	case 8713:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHAN");
		break;
	case 5980:
	case 7120:
	case 7368:
	case 7656:
	case 8592:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUANG");
		break;
	case 5931:
	case 6070:
	case 6891:
	case 7228:
	case 8366:
	case 8425:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHAO");
		break;
	case 5639:
	case 5760:
	case 6606:
	case 6860:
	case 7608:
	case 7820:
	case 8774:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHE");
		break;
	case 5837:
	case 6123:
	case 6351:
	case 6841:
	case 7309:
	case 7547:
	case 7982:
	case 8255:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHEN");
		break;
	case 6551:
	case 7441:
	case 7782:
	case 8347:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHENG");
		break;
	case 5854:
	case 5985:
	case 6110:
	case 6173:
	case 6317:
	case 7388:
	case 7459:
	case 7634:
	case 7870:
	case 8307:
	case 8334:
	case 8363:
	case 8525:
	case 8669:
	case 8685:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHI");
		break;
	case 6587:
	case 7123:
	case 8428:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHOU");
		break;
	case 5731:
	case 5951:
	case 6136:
	case 6283:
	case 6780:
	case 6888:
	case 7013:
	case 7508:
	case 7582:
	case 7988:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHU");
		break;
	case 6407:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUA");
		break;
	case 8316:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUAI");
		break;
	case 6737:
	case 6844:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUAN");
		break;
	case 7055:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUANG");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUI");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUN");
		break;
	case 6184:
	case 6287:
	case 6989:
	case 7335:
	case 7869:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUO");
		break;
	case 5643:
	case 5778:
	case 5944:
	case 6348:
	case 6765:
	case 6784:
	case 6889:
	case 7006:
	case 7065:
	case 7133:
	case 7675:
	case 7940:
	case 8024:
	case 8174:
	case 8247:
	case 8351:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SI");
		break;
	case 5801:
	case 6131:
	case 6534:
	case 6552:
	case 6676:
	case 6704:
	case 6833:
	case 8121:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SONG");
		break;
	case 5937:
	case 6220:
	case 6418:
	case 6453:
	case 6640:
	case 6849:
	case 7612:
	case 7804:
	case 7943:
	case 8284:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SOU");
		break;
	case 5777:
	case 5853:
	case 6188:
	case 6428:
	case 6726:
	case 6819:
	case 8389:
	case 8602:
	case 8653:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SU");
		break;
	case 6601:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SUAN");
		break;
	case 5839:
	case 6120:
	case 6901:
	case 6968:
	case 7661:
	case 7785:
	case 7801:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SUI");
		break;
	case 6105:
	case 6588:
	case 6624:
	case 7330:
	case 8632:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SUN");
		break;
	case 6379:
	case 6434:
	case 6442:
	case 7022:
	case 7288:
	case 7792:
	case 8440:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SUO");
		break;
	case 6743:
	case 6866:
	case 6961:
	case 7329:
	case 7719:
	case 7872:
	case 8533:
	case 8703:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TA");
		break;
	case 5902:
	case 6223:
	case 6330:
	case 7070:
	case 7536:
	case 7638:
	case 7849:
	case 8544:
	case 8656:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TAI");
		break;
	case 5916:
	case 6903:
	case 7428:
	case 7694:
	case 7867:
	case 7936:
	case 8191:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TAN");
		break;
	case 5746:
	case 6491:
	case 6871:
	case 7209:
	case 7344:
	case 7906:
	case 7959:
	case 8177:
	case 8305:
	case 8311:
	case 8442:
	case 8517:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TANG");
		break;
	case 5627:
	case 6391:
	case 6812:
	case 7226:
	case 7666:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TAO");
		break;
	case 6315:
	case 7693:
	case 7911:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TE");
		break;
	case 7588:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TENG");
		break;
	case 5735:
	case 6709:
	case 6949:
	case 7130:
	case 8035:
	case 8151:
	case 8514:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TI");
		break;
	case 6261:
	case 6735:
	case 6757:
	case 7369:
	case 7817:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TIAN");
		break;
	case 5712:
	case 7686:
	case 8127:
	case 8272:
	case 8352:
	case 8448:
	case 8622:
	case 8670:
	case 8756:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TIAO");
		break;
	case 6138:
	case 8749:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TIE");
		break;
	case 6080:
	case 6167:
	case 7035:
	case 7272:
	case 7890:
	case 8249:
	case 8610:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TING");
		break;
	case 5701:
	case 5758:
	case 6077:
	case 6444:
	case 6690:
	case 6892:
	case 7737:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TONG");
		break;
	case 7855:
	case 7822:
	case 8727:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TOU");
		break;
	case 6002:
	case 6117:
	case 6143:
	case 7842:
	case 8509:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TU");
		break;
	case 6250:
	case 6972:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TUAN");
		break;
	case 7653:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TUI");
		break;
	case 5759:
	case 6629:
	case 7453:
	case 7564:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TUN");
		break;
	case 5617:
	case 5702:
	case 5971:
	case 6653:
	case 6791:
	case 7256:
	case 7262:
	case 7350:
	case 7740:
	case 8374:
	case 8502:
	case 8541:
	case 8630:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TUO");
		break;
	case 5684:
	case 7020:
	case 7580:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WA");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WAI");
		break;
	case 5664:
	case 6025:
	case 6150:
	case 7093:
	case 7126:
	case 7194:
	case 7568:
	case 7821:
	case 8274:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WAN");
		break;
	case 5672:
	case 6244:
	case 6715:
	case 7394:
	case 8745:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WANG");
		break;
	case 5743:
	case 5835:
	case 5881:
	case 5883:
	case 6158:
	case 6217:
	case 6488:
	case 6501:
	case 6543:
	case 6545:
	case 6611:
	case 6612:
	case 6739:
	case 6777:
	case 6802:
	case 6822:
	case 6952:
	case 7024:
	case 7166:
	case 7224:
	case 7406:
	case 7631:
	case 7648:
	case 8084:
	case 8426:
	case 8659:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WEI");
		break;
	case 5656:
	case 6751:
	case 6775:
	case 7223:
	case 8609:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WEN");
		break;
	case 6178:
	case 6219:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WENG");
		break;
	case 5733:
	case 6111:
	case 6502:
	case 6855:
	case 7531:
	case 7750:
	case 8627:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WO");
		break;
	case 5603:
	case 5685:
	case 5867:
	case 5889:
	case 5956:
	case 6044:
	case 6377:
	case 6648:
	case 6668:
	case 6672:
	case 6820:
	case 6927:
	case 6935:
	case 6992:
	case 7036:
	case 7080:
	case 7227:
	case 7485:
	case 7641:
	case 8036:
	case 8045:
	case 8077:
	case 8258:
	case 8640:
	case 8789:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WU");
		break;
	case 5750:
	case 5766:
	case 5884:
	case 5913:
	case 6130:
	case 6163:
	case 6191:
	case 6241:
	case 6381:
	case 6567:
	case 6630:
	case 6750:
	case 6827:
	case 6832:
	case 6979:
	case 7050:
	case 7184:
	case 7356:
	case 7456:
	case 7474:
	case 7604:
	case 7668:
	case 7689:
	case 7691:
	case 8010:
	case 8122:
	case 8265:
	case 8303:
	case 8312:
	case 8410:
	case 8424:
	case 8443:
	case 8449:
	case 8466:
	case 8521:
	case 8791:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XI");
		break;
	case 6340:
	case 6582:
	case 6958:
	case 7206:
	case 7252:
	case 7744:
	case 8093:
	case 8333:
	case 8779:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIA");
		break;
	case 5794:
	case 5823:
	case 6040:
	case 6118:
	case 6226:
	case 6513:
	case 6593:
	case 6963:
	case 7021:
	case 7515:
	case 7662:
	case 7676:
	case 8034:
	case 8079:
	case 8225:
	case 8358:
	case 8444:
	case 8503:
	case 8548:
	case 8549:
	case 8617:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIAN");
		break;
	case 6028:
	case 6157:
	case 6635:
	case 6652:
	case 7088:
	case 7129:
	case 8313:
	case 8663:
	case 8747:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIANG");
		break;
	case 6356:
	case 6537:
	case 6876:
	case 6948:
	case 7071:
	case 7115:
	case 7241:
	case 7253:
	case 8257:
	case 8367:
	case 8379:
	case 8744:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIAO");
		break;
	case 5741:
	case 5784:
	case 5936:
	case 5938:
	case 6215:
	case 6302:
	case 6619:
	case 6661:
	case 6845:
	case 6912:
	case 6966:
	case 7105:
	case 7151:
	case 7331:
	case 7339:
	case 8583:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIE");
		break;
	case 5622:
	case 6016:
	case 7431:
	case 7607:
	case 8646:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIN");
		break;
	case 5874:
	case 6084:
	case 6309:
	case 6712:
	case 7742:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XING");
		break;
	case 6026:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIONG");
		break;
	case 6361:
	case 6522:
	case 6642:
	case 6651:
	case 6869:
	case 8028:
	case 8587:
	case 8759:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIU");
		break;
	case 5828:
	case 5935:
	case 5955:
	case 6203:
	case 6810:
	case 6851:
	case 7179:
	case 7282:
	case 7667:
	case 7776:
	case 8167:
	case 8458:
	case 8515:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XU");
		break;
	case 5756:
	case 5846:
	case 6170:
	case 6279:
	case 6789:
	case 6854:
	case 6886:
	case 7215:
	case 7324:
	case 7449:
	case 7637:
	case 7651:
	case 7759:
	case 7871:
	case 7964:
	case 8071:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XUAN");
		break;
	case 5842:
	case 7720:
	case 8529:
	case 8708:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XUE");
		break;
	case 5767:
	case 5908:
	case 5987:
	case 6087:
	case 6101:
	case 6206:
	case 6225:
	case 6530:
	case 6563:
	case 6620:
	case 6694:
	case 6813:
	case 6817:
	case 7454:
	case 8131:
	case 8524:
	case 8664:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XUN");
		break;
	case 5683:
	case 5975:
	case 6275:
	case 6512:
	case 6934:
	case 7011:
	case 7180:
	case 7266:
	case 7518:
	case 7728:
	case 7793:
	case 8073:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YA");
		break;
	case 5641:
	case 5645:
	case 5718:
	case 5740:
	case 5780:
	case 5861:
	case 5917:
	case 5919:
	case 6030:
	case 6146:
	case 6535:
	case 6691:
	case 6738:
	case 6753:
	case 6846:
	case 6857:
	case 6991:
	case 7044:
	case 7192:
	case 7360:
	case 7444:
	case 7557:
	case 7645:
	case 7827:
	case 8359:
	case 8506:
	case 8742:
	case 8748:
	case 8790:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YAN");
		break;
	case 6564:
	case 6683:
	case 7630:
	case 7640:
	case 7706:
	case 8253:
	case 8717:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YANG");
		break;
	case 5618:
	case 5619:
	case 6326:
	case 6542:
	case 6570:
	case 7159:
	case 7182:
	case 7235:
	case 7387:
	case 7455:
	case 7540:
	case 7902:
	case 8046:
	case 8126:
	case 8477:
	case 8705:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YAO");
		break;
	case 5644:
	case 5843:
	case 5894:
	case 6262:
	case 7442:
	case 7639:
	case 7884:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YE");
		break;
	case 5655:
	case 5657:
	case 5670:
	case 5693:
	case 5711:
	case 5817:
	case 5961:
	case 5992:
	case 6018:
	case 6051:
	case 6072:
	case 6218:
	case 6236:
	case 6240:
	case 6258:
	case 6314:
	case 6329:
	case 6355:
	case 6362:
	case 6441:
	case 6470:
	case 6527:
	case 6558:
	case 6602:
	case 6634:
	case 6688:
	case 6689:
	case 6708:
	case 6884:
	case 6938:
	case 7068:
	case 7143:
	case 7376:
	case 7383:
	case 7461:
	case 7629:
	case 7658:
	case 7784:
	case 7838:
	case 7955:
	case 7978:
	case 8074:
	case 8089:
	case 8115:
	case 8120:
	case 8270:
	case 8415:
	case 8464:
	case 8472:
	case 8493:
	case 8780:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YI");
		break;
	case 5623:
	case 5920:
	case 5983:
	case 6007:
	case 6065:
	case 6337:
	case 6419:
	case 6594:
	case 6625:
	case 6806:
	case 7519:
	case 7887:
	case 8111:
	case 8230:
	case 8615:
	case 8624:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YIN");
		break;
	case 5788:
	case 5911:
	case 6067:
	case 6094:
	case 6126:
	case 6151:
	case 6186:
	case 6292:
	case 6451:
	case 6663:
	case 6862:
	case 6875:
	case 6913:
	case 7188:
	case 7212:
	case 7326:
	case 7584:
	case 8048:
	case 8108:
	case 8203:
	case 8331:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YING");
		break;
	case 6401:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YO");
		break;
	case 5724:
	case 5953:
	case 6013:
	case 6415:
	case 6728:
	case 7163:
	case 7962:
	case 8014:
	case 8711:
	case 8751:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YONG");
		break;
	case 5653:
	case 5692:
	case 5707:
	case 6112:
	case 6115:
	case 6121:
	case 6347:
	case 6483:
	case 6922:
	case 7254:
	case 7364:
	case 7527:
	case 7880:
	case 8064:
	case 8236:
	case 8242:
	case 8286:
	case 8647:
	case 8778:
	case 8788:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YOU");
		break;
	case 5614:
	case 5625:
	case 5681:
	case 5722:
	case 5836:
	case 5845:
	case 6139:
	case 6187:
	case 6277:
	case 6484:
	case 6486:
	case 6546:
	case 6592:
	case 6632:
	case 6637:
	case 6655:
	case 6748:
	case 6987:
	case 6993:
	case 7005:
	case 7090:
	case 7204:
	case 7437:
	case 7476:
	case 7573:
	case 7603:
	case 7622:
	case 7647:
	case 7659:
	case 7718:
	case 7858:
	case 8033:
	case 8054:
	case 8085:
	case 8086:
	case 8130:
	case 8133:
	case 8266:
	case 8285:
	case 8336:
	case 8407:
	case 8408:
	case 8607:
	case 8625:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YU");
		break;
	case 5989:
	case 6011:
	case 6282:
	case 6768:
	case 7034:
	case 7205:
	case 7358:
	case 7528:
	case 7783:
	case 8016:
	case 8302:
	case 8378:
	case 8629:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YUAN");
		break;
	case 5763:
	case 6914:
	case 7348:
	case 7530:
	case 7865:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YUE");
		break;
	case 5909:
	case 6031:
	case 6581:
	case 6702:
	case 6719:
	case 7101:
	case 7225:
	case 7370:
	case 7432:
	case 7521:
	case 7657:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YUN");
		break;
	case 6257:
	case 6338:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZA");
		break;
	case 6544:
	case 7162:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZAI");
		break;
	case 7222:
	case 7435:
	case 8402:
	case 8456:
	case 8485:
	case 8641:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZAN");
		break;
	case 6242:
	case 7064:
	case 7416:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZANG");
		break;
	case 6380:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZAO");
		break;
	case 5638:
	case 8369:
	case 5651:
	case 6385:
	case 6493:
	case 6937:
	case 7430:
	case 8348:
	case 8423:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZE");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZEI");
		break;
	case 5858:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZEN");
		break;
	case 7153:
	case 7421:
	case 7832:
	case 7913:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZENG");
		break;
	case 6610:
	case 6274:
	case 6324:
	case 6369:
	case 6378:
	case 7736:
	case 8068:
	case 8238:
	case 8794:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHA");
		break;
	case 7746:
	case 8109:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHAI");
		break;
	case 5862:
	case 6288:
	case 7625:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHAN");
		break;
	case 5675:
	case 5921:
	case 6504:
	case 6554:
	case 6615:
	case 7049:
	case 7216:
	case 8315:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHANG");
		break;
	case 5815:
	case 7294:
	case 7840:
	case 8341:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHAO");
		break;
	case 5856:
	case 6301:
	case 7247:
	case 7392:
	case 7761:
	case 8049:
	case 8162:
	case 8256:
	case 8487:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHE");
		break;
	case 5958:
	case 6172:
	case 6805:
	case 7139:
	case 7269:
	case 7327:
	case 7384:
	case 7466:
	case 7551:
	case 7562:
	case 7685:
	case 7819:
	case 8001:
	case 8018:
	case 8380:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHEN");
		break;
	case 5826:
	case 6531:
	case 6571:
	case 7859:
	case 7903:
	case 8361:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHENG");
		break;
	case 5620:
	case 5876:
	case 5904:
	case 5990:
	case 6038:
	case 6293:
	case 6489:
	case 6669:
	case 6973:
	case 6975:
	case 7079:
	case 7246:
	case 7255:
	case 7257:
	case 7268:
	case 7382:
	case 7389:
	case 7462:
	case 7553:
	case 7589:
	case 7677:
	case 7683:
	case 7773:
	case 7984:
	case 8026:
	case 8075:
	case 8246:
	case 8474:
	case 8505:
	case 8537:
	case 8557:
	case 8560:
	case 8584:
	case 8603:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHI");
		break;
	case 5803:
	case 7981:
	case 8314:
	case 8417:
	case 8564:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHONG");
		break;
	case 6107:
	case 6390:
	case 7008:
	case 7091:
	case 7107:
	case 7548:
	case 7756:
	case 8406:
	case 8492:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHOU");
		break;
	case 5689:
	case 5710:
	case 5905:
	case 6049:
	case 6079:
	case 6808:
	case 6830:
	case 6883:
	case 7244:
	case 7338:
	case 7345:
	case 7636:
	case 7889:
	case 8070:
	case 8081:
	case 8335:
	case 8371:
	case 8422:
	case 8467:
	case 8578:
	case 8770:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHU");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUA");
		break;
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUAI");
		break;
	case 6389:
	case 6645:
	case 8207:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUAN");
		break;
	case 5755:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUANG");
		break;
	case 6723:
	case 7077:
	case 7136:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUI");
		break;
	case 7538:
	case 8124:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUN");
		break;
	case 5730:
	case 5834:
	case 6310:
	case 6823:
	case 6835:
	case 6910:
	case 7644:
	case 7690:
	case 7729:
	case 7977:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUO");
		break;
	case 5849:
	case 6549:
	case 7002:
	case 7060:
	case 7127:
	case 7287:
	case 7402:
	case 7463:
	case 7707:
	case 7786:
	case 7937:
	case 7986:
	case 8172:
	case 8342:
	case 8450:
	case 8484:
	case 8594:
	case 8604:
	case 8623:
	case 8686:
	case 8758:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZI");
		break;
	case 5744:
	case 7574:
	case 8453:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZONG");
		break;
	case 5833:
	case 5878:
	case 5924:
	case 7067:
	case 8677:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZOU");
		break;
	case 5762:
	case 6147:
	case 7963:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZU");
		break;
	case 6312:
	case 7158:
	case 8582:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZUAN");
		break;
	case 6209:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZUI");
		break;
	case 6304:
	case 7355:
	case 8714:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZUN");
		break;
	case 5872:
	case 6382:
	case 6460:
	case 6684:
	case 7549:
	case 7681:
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZUO");
		break;
	default:
		if (nCode >= 1601 && nCode <= 1602) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "A");
			break;
		}
		if (nCode >= 1603 && nCode <= 1615) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "AI");
			break;
		}
		if (nCode >= 1616 && nCode <= 1624) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "AN");
			break;
		}
		if (nCode >= 1625 && nCode <= 1627) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ANG");
			break;
		}
		if (nCode >= 1628 && nCode <= 1636) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "AO");
			break;
		}
		if (nCode >= 1637 && nCode <= 1654) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BA");
			break;
		}
		if (nCode >= 1655 && nCode <= 1662) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BAI");
			break;
		}
		if (nCode >= 1663 && nCode <= 1677) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BAN");
			break;
		}
		if (nCode >= 1678 && nCode <= 1689) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BANG");
			break;
		}
		if (nCode >= 1690 && nCode <= 1712) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BAO");
			break;
		}
		if (nCode >= 1713 && nCode <= 1727) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BEI");
			break;
		}
		if (nCode >= 1728 && nCode <= 1731) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BEN");
			break;
		}
		if (nCode >= 1732 && nCode <= 1737) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BENG");
			break;
		}
		if (nCode > 1738 && nCode <= 1761) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BI");
			break;
		}
		if (nCode >= 1762 && nCode <= 1773) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BIAN");
			break;
		}
		if (nCode >= 1774 && nCode <= 1777) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BIAO");
			break;
		}
		if (nCode >= 1778 && nCode <= 1781) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BIE");
			break;
		}
		if (nCode >= 1782 && nCode <= 1787) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BIN");
			break;
		}
		if (nCode >= 1788 && nCode <= 1794) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BING");
			break;
		}
		if (nCode >= 1801 && nCode <= 1802) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BING");
			break;
		}
		if (nCode >= 1803 && nCode <= 1821) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BO");
			break;
		}
		if (nCode >= 1822 && nCode <= 1832) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "BU");
			break;
		}
		if (nCode == 1833) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CA");
			break;
		}
		if (nCode >= 1834 && nCode <= 1844) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CAI");
			break;
		}
		if (nCode >= 1845 && nCode <= 1851) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CAN");
			break;
		}
		if (nCode >= 1852 && nCode <= 1856) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CANG");
			break;
		}
		if (nCode >= 1857 && nCode <= 1861) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CAO");
			break;
		}
		if (nCode >= 1862 && nCode <= 1866) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CE");
			break;
		}
		if (nCode >= 1867 && nCode <= 1868) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CENG");
			break;
		}
		if (nCode >= 1869 && nCode <= 1879) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHA");
			break;
		}
		if (nCode >= 1880 && nCode <= 1882) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHAI");
			break;
		}
		if (nCode >= 1883 && nCode <= 1892) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHAN");
			break;
		}
		if (nCode >= 1893 && nCode <= 1911) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHANG");
			break;
		}
		if (nCode >= 1912 && nCode <= 1920) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHAO");
			break;
		}
		if (nCode >= 1921 && nCode <= 1926) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHE");
			break;
		}
		if (nCode >= 1927 && nCode <= 1936) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHEN");
			break;
		}
		if (nCode >= 1937 && nCode <= 1951) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHENG");
			break;
		}
		if (nCode >= 1952 && nCode <= 1967) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHI");
			break;
		}
		if (nCode >= 1968 && nCode <= 1972) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHONG");
			break;
		}
		if (nCode >= 1973 && nCode <= 1984) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHOU");
			break;
		}
		if (nCode >= 1985 && nCode <= 2006) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHU");
			break;
		}
		if (nCode == 2007) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUAI");
			break;
		}
		if (nCode >= 2008 && nCode <= 2014) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUAN");
			break;
		}
		if (nCode >= 2015 && nCode <= 2020) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUANG");
			break;
		}
		if (nCode >= 2021 && nCode <= 2025) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUI");
			break;
		}
		if (nCode >= 2026 && nCode <= 2032) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUN");
			break;
		}
		if (nCode >= 2033 && nCode <= 2034) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CHUO");
			break;
		}
		if (nCode >= 2035 && nCode <= 2046) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CI");
			break;
		}
		if (nCode >= 2047 && nCode <= 2052) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CONG");
			break;
		}
		if (nCode >= 2054 && nCode <= 2057) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CU");
			break;
		}
		if (nCode >= 2058 && nCode <= 2060) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CUAN");
			break;
		}
		if (nCode >= 2061 && nCode <= 2068) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CUI");
			break;
		}
		if (nCode >= 2069 && nCode <= 2071) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CUN");
			break;
		}
		if (nCode >= 2072 && nCode <= 2077) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "CUO");
			break;
		}
		if (nCode >= 2078 && nCode <= 2083) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DA");
			break;
		}
		if (nCode >= 2084 && nCode <= 2094) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DAI");
			break;
		}
		if (nCode >= 2102 && nCode <= 2116) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DAN");
			break;
		}
		if (nCode >= 2117 && nCode <= 2121) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DANG");
			break;
		}
		if (nCode >= 2122 && nCode <= 2133) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DAO");
			break;
		}
		if (nCode >= 2134 && nCode <= 2136) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DE");
			break;
		}
		if (nCode >= 2137 && nCode <= 2143) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DENG");
			break;
		}
		if (nCode >= 2144 && nCode <= 2162) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DI");
			break;
		}
		if (nCode >= 2163 && nCode <= 2178) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DIAN");
			break;
		}
		if (nCode >= 2179 && nCode <= 2187) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DIAO");
			break;
		}
		if (nCode >= 2188 && nCode <= 2194) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DIE");
			break;
		}
		if (nCode >= 2201 && nCode <= 2209) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DING");
			break;
		}
		if (nCode == 2210) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DIU");
			break;
		}
		if (nCode >= 2211 && nCode <= 2220) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DONG");
			break;
		}
		if (nCode >= 2221 && nCode <= 2227) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DOU");
			break;
		}
		if (nCode >= 2228 && nCode <= 2242) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DU");
			break;
		}
		if (nCode >= 2243 && nCode <= 2248) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DUAN");
			break;
		}
		if (nCode >= 2249 && nCode <= 2252) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DUI");
			break;
		}
		if (nCode >= 2253 && nCode <= 2261) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DUN");
			break;
		}
		if (nCode >= 2262 && nCode <= 2273) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "DUO");
			break;
		}
		if (nCode >= 2274 && nCode <= 2286) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "E");
			break;
		}
		if (nCode == 2287) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "EN");
			break;
		}
		if (nCode >= 2288 && nCode <= 2231) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ER");
			break;
		}
		if (nCode >= 2302 && nCode <= 2309) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FA");
			break;
		}
		if (nCode >= 2310 && nCode <= 2326) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FAN");
			break;
		}
		if (nCode >= 2327 && nCode <= 2337) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FANG");
			break;
		}
		if (nCode >= 2338 && nCode <= 2349) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FEI");
			break;
		}
		if (nCode >= 2350 && nCode <= 2364) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FEN");
			break;
		}
		if (nCode >= 2365 && nCode <= 2379) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FENG");
			break;
		}
		if (nCode == 2380) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FO");
			break;
		}
		if (nCode == 2381) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FOU");
			break;
		}
		if (nCode >= 2382 && nCode <= 2432) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "FU");
			break;
		}
		if (nCode >= 2435 && nCode <= 2440) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GAI");
			break;
		}
		if (nCode >= 2441 && nCode <= 2451) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GAN");
			break;
		}
		if (nCode >= 2452 && nCode <= 2460) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GANG");
			break;
		}
		if (nCode >= 2461 && nCode <= 2470) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GAO");
			break;
		}
		if (nCode >= 2471 && nCode <= 2487) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GE");
			break;
		}
		if (nCode == 2488) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GEI");
			break;
		}
		if (nCode >= 2489 && nCode <= 2490) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GEN");
			break;
		}
		if (nCode >= 2491 && nCode <= 2503) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GENG");
			break;
		}
		if (nCode >= 2504 && nCode <= 2518) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GONG");
			break;
		}
		if (nCode >= 2519 && nCode <= 2527) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GOU");
			break;
		}
		if (nCode >= 2528 && nCode <= 2545) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GU");
			break;
		}
		if (nCode >= 2546 && nCode <= 2551) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUA");
			break;
		}
		if (nCode >= 2552 && nCode <= 2554) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUAI");
			break;
		}
		if (nCode >= 2555 && nCode <= 2565) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUAN");
			break;
		}
		if (nCode >= 2566 && nCode <= 2568) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUANG");
			break;
		}
		if (nCode >= 2569 && nCode <= 2584) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUI");
			break;
		}
		if (nCode >= 2585 && nCode <= 2587) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUN");
			break;
		}
		if (nCode >= 2588 && nCode <= 2593) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "GUO");
			break;
		}
		if (nCode == 2594) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HA");
			break;
		}
		if (nCode >= 2601 && nCode <= 2607) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HAI");
			break;
		}
		if (nCode >= 2608 && nCode <= 2626) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HAN");
			break;
		}
		if (nCode >= 2627 && nCode <= 2629) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HANG");
			break;
		}
		if (nCode >= 2630 && nCode <= 2638) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HAO");
			break;
		}
		if (nCode >= 2639 && nCode <= 2656) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HE");
			break;
		}
		if (nCode >= 2657 && nCode <= 2658) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HEI");
			break;
		}
		if (nCode >= 2659 && nCode <= 2662) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HEN");
			break;
		}
		if (nCode >= 2663 && nCode <= 2667) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HENG");
			break;
		}
		if (nCode >= 2668 && nCode <= 2676) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HONG");
			break;
		}
		if (nCode >= 2677 && nCode <= 2683) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HOU");
			break;
		}
		if (nCode >= 2684 && nCode <= 2707) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HU");
			break;
		}
		if (nCode >= 2708 && nCode <= 2716) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUA");
			break;
		}
		if (nCode >= 2717 && nCode <= 2721) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUAI");
			break;
		}
		if (nCode >= 2722 && nCode <= 2735) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUAN");
			break;
		}
		if (nCode >= 2736 && nCode <= 2749) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUANG");
			break;
		}
		if (nCode >= 2750 && nCode <= 2770) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUI");
			break;
		}
		if (nCode >= 2771 && nCode <= 2776) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUN");
			break;
		}
		if (nCode >= 2777 && nCode <= 2786) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "HUO");
			break;
		}
		if (nCode >= 2787 && nCode <= 2845) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JI");
			break;
		}
		if (nCode >= 2846 && nCode <= 2862) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIA");
			break;
		}
		if (nCode >= 2863 && nCode <= 2908) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIAN");
			break;
		}
		if (nCode >= 2909 && nCode <= 2921) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIANG");
			break;
		}
		if (nCode >= 2922 && nCode <= 2949) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIAO");
			break;
		}
		if (nCode >= 2950 && nCode <= 2976) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIE");
			break;
		}
		if (nCode >= 2977 && nCode <= 3002) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIN");
			break;
		}
		if (nCode >= 3003 && nCode <= 3027) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JING");
			break;
		}
		if (nCode >= 3028 && nCode <= 3029) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIONG");
			break;
		}
		if (nCode >= 3030 && nCode <= 3046) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JIU");
			break;
		}
		if (nCode >= 3047 && nCode <= 3071) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JU");
			break;
		}
		if (nCode >= 3072 && nCode <= 3078) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JUAN");
			break;
		}
		if (nCode >= 3079 && nCode <= 3088) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JUE");
			break;
		}
		if (nCode >= 3089 && nCode <= 3105) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "JUN");
			break;
		}
		if (nCode >= 3106 && nCode <= 3109) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KA");
			break;
		}
		if (nCode >= 3110 && nCode <= 3114) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KAI");
			break;
		}
		if (nCode >= 3115 && nCode <= 3120) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KAN");
			break;
		}
		if (nCode >= 3121 && nCode <= 3127) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KANG");
			break;
		}
		if (nCode >= 3128 && nCode <= 3131) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KAO");
			break;
		}
		if (nCode >= 3132 && nCode <= 3146) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KE");
			break;
		}
		if (nCode >= 3147 && nCode <= 3150) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KEN");
			break;
		}
		if (nCode >= 3151 && nCode <= 3152) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KENG");
			break;
		}
		if (nCode >= 3153 && nCode <= 3156) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KONG");
			break;
		}
		if (nCode >= 3157 && nCode <= 3160) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KOU");
			break;
		}
		if (nCode >= 3161 && nCode <= 3167) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KU");
			break;
		}
		if (nCode >= 3168 && nCode <= 3172) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUA");
			break;
		}
		if (nCode >= 3173 && nCode <= 3176) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUAI");
			break;
		}
		if (nCode >= 3177 && nCode <= 3178) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUAN");
			break;
		}
		if (nCode >= 3179 && nCode <= 3186) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUANG");
			break;
		}
		if (nCode >= 3187 && nCode <= 3203) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUI");
			break;
		}
		if (nCode >= 3204 && nCode <= 3207) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUN");
			break;
		}
		if (nCode >= 3208 && nCode <= 3211) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "KUO");
			break;
		}
		if (nCode >= 3212 && nCode <= 3218) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LA");
			break;
		}
		if (nCode >= 3219 && nCode <= 3221) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LAI");
			break;
		}
		if (nCode >= 3222 && nCode <= 3236) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LAN");
			break;
		}
		if (nCode >= 3237 && nCode <= 3243) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LANG");
			break;
		}
		if (nCode >= 3244 && nCode <= 3252) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LAO");
			break;
		}
		if (nCode >= 3253 && nCode <= 3254) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LE");
			break;
		}
		if (nCode >= 3255 && nCode <= 3265) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LEI");
			break;
		}
		if (nCode >= 3266 && nCode <= 3268) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LENG");
			break;
		}
		if (nCode >= 3269 && nCode <= 3308) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LI");
			break;
		}
		if (nCode == 3309) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIA");
			break;
		}
		if (nCode >= 3310 && nCode <= 3323) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIAN");
			break;
		}
		if (nCode >= 3324 && nCode <= 3334) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIANG");
			break;
		}
		if (nCode >= 3335 && nCode <= 3347) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIAO");
			break;
		}
		if (nCode >= 3348 && nCode <= 3352) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIE");
			break;
		}
		if (nCode >= 3353 && nCode <= 3363) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIN");
			break;
		}
		if (nCode >= 3364 && nCode <= 3378) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LING");
			break;
		}
		if (nCode >= 3379 && nCode <= 3389) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LIU");
			break;
		}
		if (nCode >= 3390 && nCode <= 3404) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LONG");
			break;
		}
		if (nCode >= 3405 && nCode <= 3410) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LOU");
			break;
		}
		if (nCode >= 3411 && nCode <= 3444) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LU");
			break;
		}
		if (nCode >= 3445 && nCode <= 3450) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LUAN");
			break;
		}
		if (nCode >= 3451 && nCode <= 3452) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LUE");
			break;
		}
		if (nCode >= 3453 && nCode <= 3459) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LUN");
			break;
		}
		if (nCode >= 3460 && nCode <= 3471) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "LUO");
			break;
		}
		if (nCode >= 3472 && nCode <= 3480) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MA");
			break;
		}
		if (nCode >= 3481 && nCode <= 3486) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MAI");
			break;
		}
		if (nCode >= 3487 && nCode <= 3501) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MAN");
			break;
		}
		if (nCode >= 3502 && nCode <= 3507) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MANG");
			break;
		}
		if (nCode >= 3508 && nCode <= 3519) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MAO");
			break;
		}
		if (nCode == 3520) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ME");
			break;
		}
		if (nCode >= 3521 && nCode <= 3536) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MEI");
			break;
		}
		if (nCode >= 3537 && nCode <= 3539) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MEN");
			break;
		}
		if (nCode >= 3540 && nCode <= 3547) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MENG");
			break;
		}
		if (nCode >= 3548 && nCode <= 3561) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MI");
		}
		if (nCode >= 3562 && nCode <= 3570) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIAN");
			break;
		}
		if (nCode >= 3571 && nCode <= 3578) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIAO");
			break;
		}
		if (nCode >= 3579 && nCode <= 3580) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIE");
			break;
		}
		if (nCode >= 3581 && nCode <= 3586) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIN");
			break;
		}
		if (nCode >= 3587 && nCode <= 3592) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MING");
			break;
		}
		if (nCode == 3593) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MIU");
			break;
		}
		if (nCode >= 3594 && nCode <= 3616) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MO");
			break;
		}
		if (nCode >= 3617 && nCode <= 3619) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MOU");
			break;
		}
		if (nCode >= 3620 && nCode <= 3634) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "MU");
			break;
		}
		if (nCode >= 3635 && nCode <= 3641) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NA");
			break;
		}
		if (nCode >= 3642 && nCode <= 3646) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NAI");
			break;
		}
		if (nCode >= 3647 && nCode <= 3649) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NAN");
			break;
		}
		if (nCode == 3650) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NANG");
			break;
		}
		if (nCode >= 3651 && nCode <= 3655) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NAO");
			break;
		}
		if (nCode == 3656) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NE");
			break;
		}
		if (nCode >= 3657 && nCode <= 3658) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NEI");
			break;
		}
		if (nCode == 3659) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NEN");
			break;
		}
		if (nCode == 3660) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NENG");
			break;
		}
		if (nCode >= 3661 && nCode <= 3671) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NI");
			break;
		}
		if (nCode >= 3672 && nCode <= 3678) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIAN");
			break;
		}
		if (nCode >= 3679 && nCode <= 3680) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIANG");
			break;
		}
		if (nCode >= 3681 && nCode <= 3682) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIAO");
			break;
		}
		if (nCode >= 3683 && nCode <= 3689) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIE");
			break;
		}
		if (nCode == 3690) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIN");
			break;
		}
		if (nCode >= 3691 && nCode <= 3702) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NING");
			break;
		}
		if (nCode >= 3703 && nCode <= 3706) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NIU");
			break;
		}
		if (nCode >= 3707 && nCode <= 3710) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NONG");
			break;
		}
		if (nCode >= 3711 && nCode <= 3714) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NU");
			break;
		}
		if (nCode == 3715) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NUAN");
			break;
		}
		if (nCode >= 3716 && nCode <= 3717) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NUE");
			break;
		}
		if (nCode >= 3718 && nCode <= 3721) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "NUO");
			break;
		}
		if (nCode == 3722) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "O");
			break;
		}
		if (nCode >= 3723 && nCode <= 3729) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "OU");
			break;
		}
		if (nCode >= 3730 && nCode <= 3735) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PA");
			break;
		}
		if (nCode >= 3736 && nCode <= 3741) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PAI");
			break;
		}
		if (nCode >= 3742 && nCode <= 3749) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PAN");
			break;
		}
		if (nCode >= 3750 && nCode <= 3754) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PANG");
			break;
		}
		if (nCode >= 3755 && nCode <= 3761) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PAO");
			break;
		}
		if (nCode >= 3762 && nCode <= 3770) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PEI");
			break;
		}
		if (nCode >= 3771 && nCode <= 3772) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PEN");
			break;
		}
		if (nCode >= 3773 && nCode <= 3786) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PENG");
			break;
		}
		if (nCode >= 3787 && nCode <= 3809) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PI");
			break;
		}
		if (nCode >= 3810 && nCode <= 3813) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PIAN");
			break;
		}
		if (nCode >= 3814 && nCode <= 3817) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PIAO");
			break;
		}
		if (nCode >= 3818 && nCode <= 3819) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PIE");
			break;
		}
		if (nCode >= 3820 && nCode <= 3824) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PIN");
			break;
		}
		if (nCode >= 3825 && nCode <= 3833) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PING");
			break;
		}
		if (nCode >= 3834 && nCode <= 3841) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PO");
			break;
		}
		if (nCode == 3842) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "POU");
			break;
		}
		if (nCode >= 3843 && nCode <= 3857) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "PU");
			break;
		}
		if (nCode >= 3858 && nCode <= 3893) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QI");
			break;
		}
		if (nCode == 3894 || nCode >= 3901 && nCode <= 3902) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIA");
			break;
		}
		if (nCode >= 3903 && nCode <= 3924) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIAN");
			break;
		}
		if (nCode >= 3925 && nCode <= 3932) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIANG");
			break;
		}
		if (nCode >= 3933 && nCode <= 3947) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIAO");
			break;
		}
		if (nCode >= 3948 && nCode <= 3952) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIE");
			break;
		}
		if (nCode >= 3953 && nCode <= 3963) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIN");
			break;
		}
		if (nCode >= 3964 && nCode <= 3976) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QING");
			break;
		}
		if (nCode >= 3977 && nCode <= 3978) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIONG");
			break;
		}
		if (nCode >= 3979 && nCode <= 3986) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QIU");
			break;
		}
		if (nCode >= 3987 && nCode <= 4005) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QU");
			break;
		}
		if (nCode >= 4006 && nCode <= 4016) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QUAN");
			break;
		}
		if (nCode >= 4017 && nCode <= 4024) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QUE");
			break;
		}
		if (nCode >= 4025 && nCode <= 4026) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "QUN");
			break;
		}
		if (nCode >= 4027 && nCode <= 4030) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RAN");
			break;
		}
		if (nCode >= 4031 && nCode <= 4035) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RANG");
		}
		if (nCode >= 4036 && nCode <= 4038) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RAO");
			break;
		}
		if (nCode >= 4039 && nCode <= 4040) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RE");
			break;
		}
		if (nCode >= 4041 && nCode <= 4050) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "REN");
			break;
		}
		if (nCode >= 4051 && nCode <= 4052) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RENG");
			break;
		}
		if (nCode == 4053) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RI");
			break;
		}
		if (nCode >= 4054 && nCode <= 4063) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RONG");
			break;
		}
		if (nCode >= 4064 && nCode <= 4066) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ROU");
			break;
		}
		if (nCode >= 4067 && nCode <= 4076) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RU");
			break;
		}
		if (nCode >= 4077 && nCode <= 4078) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RUAN");
			break;
		}
		if (nCode >= 4079 && nCode <= 4081) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RUI");
			break;
		}
		if (nCode >= 4082 && nCode <= 4083) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RUN");
			break;
		}
		if (nCode >= 4084 && nCode <= 4085) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "RUO");
			break;
		}
		if (nCode >= 4086 && nCode <= 4088) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SA");
			break;
		}
		if (nCode >= 4089 && nCode <= 4092) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SAI");
			break;
		}
		if (nCode >= 4093 && nCode <= 4094) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SAN");
			break;
		}
		if (nCode >= 4101 && nCode <= 4102) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SAN");
			break;
		}
		if (nCode >= 4103 && nCode <= 4105) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SANG");
			break;
		}
		if (nCode >= 4106 && nCode <= 4109) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SAO");
			break;
		}
		if (nCode >= 4110 && nCode <= 4112) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SE");
			break;
		}
		if (nCode == 4113) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SEN");
		}
		if (nCode == 4114) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SENG");
			break;
		}
		if (nCode >= 4115 && nCode <= 4123) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHA");
			break;
		}
		if (nCode >= 4124 && nCode <= 4125) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHAI");
			break;
		}
		if (nCode >= 4126 && nCode <= 4141) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHAN");
			break;
		}
		if (nCode >= 4142 && nCode <= 4149) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHANG");
			break;
		}
		if (nCode >= 4150 && nCode <= 4160) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHAO");
			break;
		}
		if (nCode >= 4161 && nCode <= 4172) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHE");
			break;
		}
		if (nCode >= 4173 && nCode <= 4188) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHEN");
			break;
		}
		if (nCode >= 4189 && nCode <= 4205) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHENG");
			break;
		}
		if (nCode >= 4206 && nCode <= 4252) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHI");
			break;
		}
		if (nCode >= 4253 && nCode <= 4262) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHOU");
			break;
		}
		if (nCode >= 4263 && nCode <= 4301) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHU");
			break;
		}
		if (nCode >= 4302 && nCode <= 4303) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUA");
			break;
		}
		if (nCode >= 4304 && nCode <= 4307) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUAI");
			break;
		}
		if (nCode >= 4308 && nCode <= 4309) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUAN");
			break;
		}
		if (nCode >= 4310 && nCode <= 4312) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUANG");
			break;
		}
		if (nCode >= 4313 && nCode <= 4316) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUI");
			break;
		}
		if (nCode >= 4317 && nCode <= 4320) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUN");
			break;
		}
		if (nCode >= 4321 && nCode <= 4324) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SHUO");
			break;
		}
		if (nCode >= 4325 && nCode <= 4340) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SI");
			break;
		}
		if (nCode >= 4341 && nCode <= 4348) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SONG");
			break;
		}
		if (nCode >= 4349 && nCode <= 4352) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SOU");
			break;
		}
		if (nCode >= 4353 && nCode <= 4364) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SU");
			break;
		}
		if (nCode >= 4365 && nCode <= 4367) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SUAN");
			break;
		}
		if (nCode >= 4368 && nCode <= 4378) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SUI");
			break;
		}
		if (nCode >= 4379 && nCode <= 4381) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SUN");
			break;
		}
		if (nCode >= 4382 && nCode <= 4389) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "SUO");
			break;
		}
		if (nCode >= 4390 && nCode <= 4404) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TA");
			break;
		}
		if (nCode >= 4405 && nCode <= 4413) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TAI");
			break;
		}
		if (nCode >= 4414 && nCode <= 4431) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TAN");
			break;
		}
		if (nCode >= 4432 && nCode <= 4444) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TANG");
			break;
		}
		if (nCode >= 4445 && nCode <= 4455) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TAO");
			break;
		}
		if (nCode == 4456) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TE");
			break;
		}
		if (nCode >= 4457 && nCode <= 4460) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TENG");
			break;
		}
		if (nCode >= 4461 && nCode <= 4475) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TI");
			break;
		}
		if (nCode >= 4476 && nCode <= 4483) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TIAN");
			break;
		}
		if (nCode >= 4484 && nCode <= 4488) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TIAO");
			break;
		}
		if (nCode >= 4489 && nCode <= 4491) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TIE");
			break;
		}
		if (nCode >= 4492 && nCode <= 4507) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TING");
			break;
		}
		if (nCode >= 4508 && nCode <= 4520) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TONG");
			break;
		}
		if (nCode >= 4521 && nCode <= 4524) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TOU");
			break;
		}
		if (nCode >= 4525 && nCode <= 4535) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TU");
			break;
		}
		if (nCode >= 4536 && nCode <= 4537) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TUAN");
			break;
		}
		if (nCode >= 4538 && nCode <= 4543) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TUI");
			break;
		}
		if (nCode >= 4544 && nCode <= 4546) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TUN");
			break;
		}
		if (nCode >= 4547 && nCode <= 4557) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "TUO");
			break;
		}
		if (nCode >= 4558 && nCode <= 4564) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WA");
			break;
		}
		if (nCode >= 4565 && nCode <= 4566) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WAI");
			break;
		}
		if (nCode >= 4567 && nCode <= 4583) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WAN");
			break;
		}
		if (nCode >= 4584 && nCode <= 4593) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WANG");
			break;
		}
		if (nCode >= 4594 && nCode <= 4632) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WEI");
			break;
		}
		if (nCode >= 4633 && nCode <= 4642) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WEN");
			break;
		}
		if (nCode >= 4643 && nCode <= 4645) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WENG");
			break;
		}
		if (nCode >= 4646 && nCode <= 4654) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WO");
			break;
		}
		if (nCode >= 4655 && nCode <= 4683) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "WU");
			break;
		}
		if (nCode >= 4684 && nCode <= 4724) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XI");
			break;
		}
		if (nCode >= 4725 && nCode <= 4737) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIA");
			break;
		}
		if (nCode >= 4738 && nCode <= 4763) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIAN");
			break;
		}
		if (nCode >= 4764 && nCode <= 4783) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIANG");
			break;
		}
		if (nCode >= 4784 && nCode <= 4807) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIAO");
			break;
		}
		if (nCode >= 4809 && nCode <= 4828) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIE");
			break;
		}
		if (nCode >= 4829 && nCode <= 4838) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIN");
			break;
		}
		if (nCode >= 4839 && nCode <= 4853) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XING");
			break;
		}
		if (nCode >= 4854 && nCode <= 4860) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIONG");
			break;
		}
		if (nCode >= 4861 && nCode <= 4869) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XIU");
			break;
		}
		if (nCode >= 4870 && nCode <= 4888) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XU");
			break;
		}
		if (nCode >= 4889 && nCode <= 4904) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XUAN");
			break;
		}
		if (nCode >= 4905 && nCode <= 4910) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XUE");
			break;
		}
		if (nCode >= 4911 && nCode <= 4924) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "XUN");
			break;
		}
		if (nCode >= 4925 && nCode <= 4940) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YA");
			break;
		}
		if (nCode >= 4941 && nCode <= 4973) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YAN");
			break;
		}
		if (nCode >= 4974 && nCode <= 4990) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YANG");
			break;
		}
		if (nCode >= 4991 && nCode <= 5011) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YAO");
			break;
		}
		if (nCode >= 5012 && nCode <= 5026) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YE");
			break;
		}
		if (nCode >= 5027 && nCode <= 5079) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YI");
			break;
		}
		if (nCode >= 5080 && nCode <= 5101) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YIN");
			break;
		}
		if (nCode >= 5102 && nCode <= 5119) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YING");
			break;
		}
		if (nCode == 5120) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YO");
			break;
		}
		if (nCode >= 5121 && nCode <= 5135) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YONG");
			break;
		}
		if (nCode >= 5136 && nCode <= 5155) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YOU");
			break;
		}
		if (nCode >= 5156 && nCode <= 5206) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YU");
			break;
		}
		if (nCode >= 5207 && nCode <= 5226) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YUAN");
			break;
		}
		if (nCode >= 5227 && nCode <= 5236) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YUE");
			break;
		}
		if (nCode >= 5237 && nCode <= 5248) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "YUN");
			break;
		}
		if (nCode >= 5249 && nCode <= 5251) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZA");
			break;
		}
		if (nCode >= 5252 && nCode <= 5258) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZAI");
			break;
		}
		if (nCode >= 5259 && nCode <= 5262) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZAN");
			break;
		}
		if (nCode >= 5263 && nCode <= 5265) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZANG");
			break;
		}
		if (nCode >= 5266 && nCode <= 5279) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZAO");
			break;
		}
		if (nCode >= 5280 && nCode <= 5283) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZE");
			break;
		}
		if (nCode == 5284) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZEI");
			break;
		}
		if (nCode == 5285) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZEN");
			break;
		}
		if (nCode >= 5286 && nCode <= 5289) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZENG");
			break;
		}
		if (nCode >= 5290 && nCode <= 5309) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHA");
			break;
		}
		if (nCode >= 5310 && nCode <= 5315) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHAI");
			break;
		}
		if (nCode >= 5316 && nCode <= 5332) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHAN");
			break;
		}
		if (nCode >= 5333 && nCode <= 5347) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHANG");
			break;
		}
		if (nCode >= 5348 && nCode <= 5357) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHAO");
			break;
		}
		if (nCode >= 5358 && nCode <= 5367) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHE");
			break;
		}
		if (nCode >= 5368 && nCode <= 5383) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHEN");
			break;
		}
		if (nCode >= 5384 && nCode <= 5404) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHENG");
			break;
		}
		if (nCode >= 5405 && nCode <= 5447) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHI");
			break;
		}
		if (nCode >= 5448 && nCode <= 5458) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHONG");
			break;
		}
		if (nCode >= 5459 && nCode <= 5472) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHOU");
			break;
		}
		if (nCode >= 5473 && nCode <= 5504) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHU");
			break;
		}
		if (nCode >= 5505 && nCode <= 5506) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUA");
			break;
		}
		if (nCode == 5507) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUAI");
			break;
		}
		if (nCode >= 5508 && nCode <= 5513) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUAN");
			break;
		}
		if (nCode >= 5514 && nCode <= 5520) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUANG");
			break;
		}
		if (nCode >= 5521 && nCode <= 5526) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUI");
			break;
		}
		if (nCode >= 5527 && nCode <= 5528) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUN");
			break;
		}
		if (nCode >= 5529 && nCode <= 5539) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZHUO");
			break;
		}
		if (nCode >= 5540 && nCode <= 5554) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZI");
			break;
		}
		if (nCode >= 5555 && nCode <= 5561) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZONG");
			break;
		}
		if (nCode >= 5562 && nCode <= 5565) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZOU");
			break;
		}
		if (nCode >= 5566 && nCode <= 5573) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZU");
			break;
		}
		if (nCode >= 5574 && nCode <= 5575) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZUAN");
			break;
		}
		if (nCode >= 5576 && nCode <= 5579) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZUI");
			break;
		}
		if (nCode >= 5580 && nCode <= 5581) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZUN");
			break;
		}
		if (nCode >= 5582 && nCode <= 5589) {
			strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "ZUO");
			break;
		}
		strcpy_s(strValue, HZ2PY_MAX_PINYIN_SIZE, "?");
		return 0;
	}
	return 1;
}
void pinyin_gb2312(char* inbuf, char* outbuf, bool m_blnSimaple = false, bool polyphone_support = false, bool m_blnFirstBig = false, bool m_blnAllBiG = false, bool m_LetterEnd = false, bool m_unknowSkip = true, bool m_filterPunc = true) {
	/*
	bool m_blnSimaple=false; //是否简拼
	bool m_blnFirstBig=false; //拼音第一个字母是否大写
	bool m_blnAllBiG=false; //拼音是否大写
	bool m_LetterEnd=false; //每个拼音后是否用分隔符
	bool m_unknowSkip=false; //未知拼音的字是否略过
	bool m_filterPunc=true; //是否过滤标点
	*/
	unsigned char ucHigh, ucLow;
	int nCode, j, k;
	int inbuf_len = strlen(inbuf);
	int iOutbuf = 0;
	char pinyinValue[HZ2PY_MAX_PINYIN_SIZE];
	char sep = '\'';
	for (int i = 0; i < inbuf_len; i++) {
		if ((unsigned char)inbuf[i] < 0x80) {
			if (m_filterPunc && !(inbuf[i] >= 'a' && inbuf[i] <= 'z' || inbuf[i] >= 'A' && inbuf[i] <= 'Z')) {
				continue;
			}
			if (!safeAddToOutbuf(outbuf, iOutbuf, &inbuf[i], 1))
				return;
			//
			if (m_LetterEnd)
				if (!safeAddToOutbuf(outbuf, iOutbuf, &sep, 1))
					return;
			//英文字母也打分隔符
			continue;
		}
		ucHigh = (unsigned char)inbuf[i];
		ucLow = (unsigned char)inbuf[i + 1];
		if (ucHigh < 0xa1 || ucLow < 0xa1)
			continue;
		else
			nCode = (ucHigh - 0xa0) * 100 + ucLow - 0xa0;
		int strRes = findLetter(nCode, pinyinValue);
		if (m_unknowSkip && !strRes) {
			i++;
			continue;
		}
		if (!strRes) {
			if (!safeAddToOutbuf(outbuf, iOutbuf, &inbuf[i], 2))
				return;
			i++;
			continue;
		}
		if (!polyphone_support) { //不支持多音字
			k = 0;
			while (pinyinValue[k] != '\0') {
				if (pinyinValue[k] == '|') {
					pinyinValue[k] = '\0';
					break;
				}
				k++;
			}
		}
		if (m_blnSimaple) //只支持首字母
		{
			k = 1;
			j = 1;
			while (pinyinValue[k] != '\0') {
				if (pinyinValue[k] == '|' || pinyinValue[k - 1] == '|') {
					pinyinValue[j] = pinyinValue[k];
					j++;
				}
				k++;
			}
			pinyinValue[j] = '\0'; //只取第一个字符
		}
		int iPinyinValue = strlen(pinyinValue);
		if (!m_blnFirstBig) //首字母是否大写
		{
			pinyinValue[0] = pinyinValue[0] + 32;
			for (j = 1; j < iPinyinValue; j++) {
				if (pinyinValue[j - 1] == '|' && pinyinValue[j] >= 'A' && pinyinValue[j] <= 'Z') {
					pinyinValue[j] = pinyinValue[j] + 32;
				}
			}
		}
		if (!m_blnAllBiG) {
			for (j = 1; j < iPinyinValue; j++) {
				if (pinyinValue[j] >= 'A' && pinyinValue[j] <= 'Z') { //strRes.SetAt(j, (strRes.GetAt(j) + 32));
					pinyinValue[j] = pinyinValue[j] + 32;
				}
			}
		}
		if (!safeAddToOutbuf(outbuf, iOutbuf, pinyinValue, iPinyinValue))
			return;
		i++;
		if (m_LetterEnd) if (!safeAddToOutbuf(outbuf, iOutbuf, &sep, 1))
			return;
	}
}

