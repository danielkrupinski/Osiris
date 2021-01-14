#include <cstdio>
#include <vector>
#include <memory>
#include <string>
#include <deque>

namespace valve_parser
{
	class Document;
	class KeyValue;
	class Object;

	enum ENCODING
	{
		UTF8,
		UTF16_LE,
		UTF16_BE,
		UTF32_LE,
		UTF32_BE,
		MAX
	};

	enum
	{
		STRING = '"',
		OBJECT_OPEN = '{',
		OBJECT_CLOSE = '}',
		COMMENT = '/',
	};


	class Str
	{
	public:
		char32_t* _start = 0;
		char32_t* _end = 0;
	public:
		std::string toString()
		{
			if (!_start || !_end)
				return "";

			//convert utf32 to utf16
			std::vector<char16_t> utf16;
			utf16.reserve(200);

			for (char32_t* text = _start; text != _end + 1; text++)
			{
				if ((*text >= 0 && *text <= 0xD7FF) ||
					*text >= 0xE000 && *text <= 0xFFFF)
				{
					utf16.push_back(*text);
				}
				if (*text >= 0x10000 && *text <= 0x10FFFF)
				{
					char32_t offset = *text - 0x10000;
					char16_t hi = (offset & 0xFFC00) >> 10;
					char16_t lo = offset & 0x3FF;
					hi += 0xD800;
					lo += 0xDC00;
					utf16.push_back(hi);
					utf16.push_back(lo);
				}
			}
			return std::string(utf16.begin(), utf16.end());
		}

		//warning: no exception handling
		int toInt()
		{
			return std::stoi(toString());
		}

		//checks whether the next read character equals expectedTag, if not 0 is returned
		static char32_t* ParseTextExpectedTag(char32_t* p, char32_t expectedTag, bool IGNORE_SPACE_TAB_LF_CR)
		{
			for (; *p; p++)
			{
				//skip comment line
				if (*p == COMMENT && *(p + 1) && *(p + 1) == COMMENT)
				{
					for (; *p && *p != '\n'; p++);
				}

				//skip options line
				// [§xbox|§360]
				if (*p == '[')
				{
					for (; *p && *p != ']'; p++);
					p++;
				}

				if (IGNORE_SPACE_TAB_LF_CR)
				{
					if (*p == '\n' || *p == '\r' || *p == '\t' || *p == ' ')
						continue;
				}

				if (*p == expectedTag)
					return p;
				else
					return 0;
			}

			return 0;
		}

		static char32_t* EndReached(char32_t* p, char32_t expectedTag)
		{
			for (; *p; p++)
			{
				//skip comment line
				if (*p == COMMENT && *(p + 1) && *(p + 1) == COMMENT)
				{
					for (; *p && *p != '\n'; p++);
				}

				//skip options line
				// [§xbox|§360]
				if (*p == '[')
				{
					for (; *p && *p != ']'; p++);
					p++;
				}

				if (*p == '\n' || *p == '\r' || *p == '\t' || *p == ' ')
					continue;

				if (*p == expectedTag)
					return p;
				else
					return (char32_t*)-1;
			}

			return 0;
		}

		//reads string until it ecnounters endTag
		static char32_t* ParseTextEndTag(char32_t* p, char32_t endTag)
		{
			for (; *p; p++)
			{
				//skip escape sequence
				//there are also hex escapes
				//\xAA45, no need to handle them tho
				if (*p == '\\')
					p += 2;

				if (*p == endTag)
					return p;
			}

			return 0;
		}
	};

	class Util
	{
	public:
		static bool StrEqu(Str str1, char* str2)
		{
			return str1.toString() == std::string(str2);
		}

		static bool StrEquI(Str str1, char* str2)
		{
			std::string str = str1.toString();
			return str.compare(str2) == std::string::npos;
		}
	};

	class Node
	{
	protected:
		Document* _doc;
	public:
		std::vector<std::shared_ptr<Node>> children;
		virtual bool Parse();

		Node(Document* doc)
		{
			_doc = doc;
		}

		virtual Object* ToObject()
		{
			return 0;
		}

		virtual KeyValue* ToKeyValue()
		{
			return 0;
		}
	};

	class KeyValue : public Node
	{
	public:
		Str Key;
		Str Value;

		KeyValue(Document* doc) : Node(doc) {}

		KeyValue* ToKeyValue()
		{
			return this;
		}
	};

	class Object : public Node
	{
	public:
		Str name;
		Object(Document* doc);
		Object* ToObject();
		bool Parse();
		std::shared_ptr<Node> GetKeyByName(char* name);
	};

	class Document
	{
	private:
		friend class Object;
		friend class Node;

		struct BOM
		{
			char* str;
			size_t len;
		};
		BOM BOMS[ENCODING::MAX];

		char32_t* utf32text = 0;
		char32_t* p;
	public:
		std::shared_ptr<Node> root;

		

		~Document()
		{
			if (utf32text)
				delete[] utf32text;
		}

		std::shared_ptr<Node> BreadthFirstSearch(char* name, bool caseInsensitive = false)
		{
			std::deque<std::shared_ptr<Node>> q;
			q.push_back(root);
			while (!q.empty())
			{
				std::shared_ptr<Node> f = q.front();
				q.pop_front();

				if (f->ToKeyValue())
				{
					if (!caseInsensitive && Util::StrEqu(f->ToKeyValue()->Key, name))
						return f;

					if (caseInsensitive && Util::StrEquI(f->ToKeyValue()->Key, name))
						return f;
				}
				if (f->ToObject())
				{
					if (!caseInsensitive && Util::StrEqu(f->ToObject()->name, name))
						return f;

					if (caseInsensitive && Util::StrEquI(f->ToObject()->name, name))
						return f;
				}

				for (auto& child : f->children)
					q.push_back(child);
			}

			return 0;
		}

		std::vector<std::shared_ptr<Node>> BreadthFirstSearchMultiple(char* name, bool caseInsensitive = false)
		{
			std::vector<std::shared_ptr<Node>> vec;
			std::deque<std::shared_ptr<Node>> q;
			q.push_back(root);
			while (!q.empty())
			{
				std::shared_ptr<Node> f = q.front();
				q.pop_front();

				if (f->ToKeyValue())
				{
					if (!caseInsensitive && Util::StrEqu(f->ToKeyValue()->Key, name))
						vec.push_back(f);

					if (caseInsensitive && Util::StrEquI(f->ToKeyValue()->Key, name))
						vec.push_back(f);
				}
				if (f->ToObject())
				{
					if (!caseInsensitive && Util::StrEqu(f->ToObject()->name, name))
						vec.push_back(f);

					if (caseInsensitive && Util::StrEquI(f->ToObject()->name, name))
						vec.push_back(f);
				}

				for (auto& child : f->children)
					q.push_back(child);
			}

			return vec;
		}

		//returns true when format is correct
		//parse from file
		bool Load(char* path, ENCODING encoding)
		{
			auto f = fopen(path, "rb");
			if (!f)
				return false;

			fseek(f, 0, SEEK_END);
			auto size = ftell(f);
			if (!size)
				return false;

			size_t null_terminator_bytes = 0;
			if (encoding == ENCODING::UTF16_BE ||
				encoding == ENCODING::UTF16_LE)
				null_terminator_bytes = 2;

			if (encoding == ENCODING::UTF8)
				null_terminator_bytes = 1;

			if (encoding == ENCODING::UTF32_BE ||
				encoding == ENCODING::UTF32_LE)
				null_terminator_bytes = 4;

			char* _text = new char[size + null_terminator_bytes];
			fseek(f, 0, SEEK_SET);
			if (fread(_text, 1, size, f) != size)
				return false;

			for (size_t i = 0; i < null_terminator_bytes; i++)
				_text[size + i] = 0;

			fclose(f);

			//convert to utf32
			//which makes everything easy to handle
			//but increases buffer size
			cnv_to_utf32(_text, encoding);
			delete[] _text;

			p = trimSpace(utf32text);
			if (!p)
				return false;

			return ParseDeep();
		}
	private:
		char* skipBOM(char* p, ENCODING encoding)
		{
			if (!memcmp(p, BOMS[encoding].str, BOMS[encoding].len))
				return p + BOMS[encoding].len;
			else
				return p;
		}

		void cnv_to_utf32(char* text, ENCODING encoding)
		{
			if (encoding == ENCODING::UTF16_LE)
			{
				char* bom = skipBOM(text, ENCODING::UTF16_LE);
				cnv_utf16_le(bom);
			}

			if (encoding == ENCODING::UTF16_BE)
			{
				char* bom = skipBOM(text, ENCODING::UTF16_BE);
				cnv_utf16_be(bom);
			}

			if (encoding == ENCODING::UTF8)
			{
				char* bom = skipBOM(text, ENCODING::UTF8);
				cnv_utf8(bom);
			}

			if (encoding == ENCODING::UTF32_LE)
			{
				char* bom = skipBOM(text, ENCODING::UTF32_LE);
				cnv_utf32_le(bom);
			}

			if (encoding == ENCODING::UTF32_BE)
			{
				char* bom = skipBOM(text, ENCODING::UTF32_BE);
				cnv_utf32_be(bom);
			}
		}

		size_t cnv_utf16_be_len(char* text)
		{
			size_t len = 0;
			char16_t* u16text = (char16_t*)text;
			while (*u16text)
			{

				char16_t c = 0;
				char* t = (char*)u16text;
				c |= (char16_t)*(t + 1) << 8;
				c |= *t;

				//4 bytes
				if (c >= 0xD800 && c <= 0xDFFF)
					u16text++;

				len++;
				u16text++;
			}

			return len;
		}

		size_t cnv_utf16_le_len(char* text)
		{
			size_t len = 0;
			char16_t* u16text = (char16_t*)text;
			while (*u16text)
			{
				if (*u16text >= 0xD800 && *u16text <= 0xDFFF)
					u16text++;

				len++;
				u16text++;
			}

			return len;
		}

		void cnv_utf16_be(char* text)
		{
			auto s = cnv_utf16_be_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			char16_t* u16text = (char16_t*)text;
			char32_t k;

			while (*u16text)
			{

				char16_t c = 0;
				char* t = (char*)u16text;
				c |= (char16_t)*(t + 1) << 8;
				c |= *t;

				//4 bytes
				if (c >= 0xD800 && c <= 0xDFFF)
				{
					char16_t hi = c - 0xD800;

					t = (char*)(u16text + 1);
					c |= (char16_t)*(t + 1) << 8;
					c |= *t;
					char16_t lo = c - 0xDC00;

					k = (char32_t)(hi & 0x3FF) << 10;
					k |= lo & 0x3FF;
					k += 0x10000;
					u16text++;
				}
				else //2 bytes
				{
					k = c;
				}
				utf32text[i] = k;
				i++;
				u16text++;
			}
		}

		void cnv_utf16_le(char* text)
		{
			auto s = cnv_utf16_le_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			char16_t* u16text = (char16_t*)text;
			char32_t k;

			while (*u16text)
			{
				//4 bytes
				if (*u16text >= 0xD800 && *u16text <= 0xDFFF)
				{
					char16_t hi = *u16text - 0xD800;
					char16_t lo = *(u16text + 1) - 0xDC00;
					k = (char32_t)(hi & 0x3FF) << 10;
					k |= lo & 0x3FF;
					k += 0x10000;
					u16text++;
				}
				else //2 bytes
				{
					k = *u16text;
				}
				utf32text[i] = k;
				i++;
				u16text++;
			}
		}

		size_t cnv_utf8_len(char* text)
		{
			size_t len = 0;
			unsigned char c = (unsigned char)*text;

			while (c)
			{
				if (c >= 0xc0 && c <= 0xdf)
				{
					text++;
				}
				if (c >= 0xe0 && c <= 0xef)
				{
					text += 2;
				}
				if (c >= 0xf0 && c <= 0xf7)
				{
					text += 3;
				}

				len++;
				c = *++text;
			}

			return len;
		}

		void cnv_utf8(char* text)
		{
			auto s = cnv_utf8_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			unsigned char c = (unsigned char)*text;

			while (c)
			{
				char32_t k = 0;
				if (c >= 0 && c <= 0x7f)
				{
					k = c;
				}
				if (c >= 0xc0 && c <= 0xdf)
				{
					k = (char32_t)(c ^ 0xc0) << 6;
					c = *++text;
					k |= c ^ 0x80;
				}
				if (c >= 0xe0 && c <= 0xef)
				{
					k = (char32_t)(c ^ 0xe0) << 12;
					c = *++text;
					k |= (char32_t)(c ^ 0x80) << 6;
					c = *++text;
					k |= c ^ 0x80;
				}
				if (c >= 0xf0 && c <= 0xf7)
				{
					k = (char32_t)(c ^ 0xf0) << 18;
					c = *++text;
					k |= (char32_t)(c ^ 0x80) << 12;
					c = *++text;
					k |= (char32_t)(c ^ 0x80) << 6;
					c = *++text;
					k |= c ^ 0x80;
				}
				utf32text[i] = k;
				i++;
				c = *++text;
			}
		}

		size_t cnv_utf32_len(char* text)
		{
			size_t len = 0;
			char32_t* p = (char32_t*)text;

			while (*p)
			{
				len++;
				p++;
			}

			return len;
		}

		void cnv_utf32_le(char* text)
		{
			auto s = cnv_utf32_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			char32_t* p = (char32_t*)text;

			while (*p)
			{
				utf32text[i] = *p;
				i++;
				p++;
			}
		}

		void cnv_utf32_be(char* text)
		{
			auto s = cnv_utf32_len(text);
			utf32text = new char32_t[s + 1];
			utf32text[s] = 0;
			int i = 0;

			char32_t* p = (char32_t*)text;
			char32_t k;

			while (*p)
			{
				char* t = (char*)p;
				k = (char32_t)*(t + 3) << 24;
				k |= (char32_t)*(t + 2) << 16;
				k |= (char32_t)*(t + 1) << 8;
				k |= *t;

				utf32text[i] = k;
				i++;
				p++;
			}
		}

		char32_t* trimSpace(char32_t* p)
		{
			while (*p)
			{
				if (*p == ' ')
					p++;
				else
					return p;
			}

			return 0;
		}

		bool identify(std::shared_ptr<Node>& node)
		{
			auto string_begin = Str::ParseTextExpectedTag(p, STRING, true);
			if (!string_begin)
				return false;

			auto string_end = Str::ParseTextEndTag(string_begin + 1, STRING);
			if (!string_end)
				return false;


			auto object_open = Str::ParseTextExpectedTag(string_end + 1, OBJECT_OPEN, true);
			auto val_start = Str::ParseTextExpectedTag(string_end + 1, STRING, true);

			if (!object_open && !val_start)
				return false;

			if (object_open)
			{
				std::shared_ptr<Object> obj = std::make_shared<Object>(this);
				obj->name._start = string_begin + 1;
				obj->name._end = string_end - 1;
				node = obj;
				p = object_open + 1;
				return true;
			}

			if (val_start)
			{
				auto val_end = Str::ParseTextEndTag(val_start + 1, STRING);
				if (!val_end)
					return false;

				std::shared_ptr<KeyValue> keyVal = std::make_shared<KeyValue>(this);
				keyVal->Key._start = string_begin + 1;
				keyVal->Key._end = string_end - 1;
				keyVal->Value._start = val_start + 1;
				keyVal->Value._end = val_end - 1;
				node = keyVal;
				p = val_end + 1;
				return true;
			}
		}

		bool ParseDeep()
		{
			root = std::make_shared<Node>(this);
			return root->Parse();
		}
	};
}




static const char smalll_compressed_data_base85[32600 + 1] =
"7])#######JovG?'/###[),##0rC$#Q6>##R%_O;GjNT#Ga9SIsA'o/fr;x7=(;ehx)m<-3B^01O[n427>j6=h->>#CEnB4aNV=Barb)ekYqr$Y?uu#Wa(*Hlme+M]s:SIxC)m0#-0%J"
"U%k.pk:jl&f823C_,d<B'-/5[HRt-$f'TqLNqU976]-R<_om92E3n0Fv0D1EP]^w'nO)##'`#iFYLHrP/D+##YR$##_cV=BG4ZY5TQi---1o92?4^=BmB>O^@h(##:LHwKNvAiFGBOMY"
"P=%q.`+*##F6sQj6@spoGPFZAeI1=#c1f+Mq<(q#2;FF%re?in*x/PA;N&m]LH@v$sLZn*[5+w$5].-#4p4oLo5rf%HKM.M<>pV-AnFX(QM#lLV;#gLH$=/1H@_f#+BP##%V%h$9P*B-"
"HIRm/_5WfC7Y###GQ7fMVH]0#sSV+#UEB@'KYu.:ZEG&#?W&]toxGju^/g2%S<cwL++p+MjOH##LD1F%0F$?$`l-qLP@?>#R^&.$[GP##r:_'#FF>##V?`$#daVmLv4W7#_$u.#cPX&#"
"4w'hL.K&0#j@I8#L]r,#8Q,+#agR%#F&6tLl+q=#V%auL[RGgL[=:=#GJM,#Pku##Fxo=#;@m3#(66;#@$)t-:#[iLTM^-#*NZ;#<H_hL3:Y9#BOH&M$%k4#]ZU).u94GMxx_%MxEUQ$"
"/6S>-7gF?-w_>W-i:p9;`51R<=&7R3q9Dig%O(Pf8(Fk4K_K`tj`=W%o);,M0qugL32N*M=dm)M*ke;#mm$0#gfl)MoPm`MNYoo#s@uu#&uqtL@pv.q>HXH3'Lx+Vm/LSnlcZSf/cU_/"
"D13F%_N7wgFkw%kj6%BlUr)j'0RjiTm`?gM)ve`NSP4/`M`d9/JeM4#QHj'M-A6)M8(/0#<es)#^G4v-=H9sL)RM/#.$ugL$/2PMMmbP$e.[mL$r^M'r)mfL+lL5/$E)4#s'brL)e9=#"
"jN8=#EPff.&WSX#7MPH2'1###0#b1#h_N1#GCC/#IB;=-4fe6/o6YY#3K./N`%g+MpYpuG-R8/UqYN'#)*NxtSlK##(JJR*%de-6(^Qxb++)##3`/,M>KLlo-lq;$:=)W%;?:@-/&kB-"
"VsP8.b.(##-x,Z$ILwA-05T;-3/kfLd4,.%8_-Z$YS(IOwc*2g2J&mAWIhxlP)XPT7x,Z$6Z`=-R6T;-capV-)bBwK+og>$+&kB-=Lx>-$.Q8.uM7)$7Uh>$w`gDOYw5/C_K-F%N55##"
":/###UCT;-%u:T.Y:>>#b6T;-n5T;-VeuH25`w8#K,v+#d;###R8h'#.7T;-LG*j0x/LGVa<Grd,Z#8IN.0m9ehbP9+F1KiW].JLK[PrZ2C'NC$&AuYc2bJ2nZvJ;D/RfL&pjIh13l=c"
"^eb9MAoZc;gZsr$DFEG2V1@SI&XS/.X$:hL=0_a#Ml3$M/xe2N_fSD$fg(P-^lL5/xiHO$^ug%Mm_4P$0i82%FGm]MDBC*#8G&=#&*$;#J*m<->O#<-8[lS.VtX.#C0r5/:rf=#v94GM"
"f_e&M-MwA-d4_W.3XW)#cYkV.aqX.#Clw:0LJD,#-B?;#q@g9;>&_%M4_C:#$XjY.&X9:#)U<^-0I&:)ARKwKPWd7eVml:dZ;QWe=$1Vd2s`4fMb9_A_RFwK',j:dL8('cOl*SewWOxb"
"=Vl-$a3vQN;k/F%,F/R<(pp-$]8&PfZj5Yc9bjQaJe4]b76)PflZ/R<`il-$_fk--M#Tw9%c'^#V8E5^dqUw93I-^+AWslTa[kwZDEsY#VW0Ab$N*8[U5W(s2-@wgc)78#uUB#$0cs-$"
"hG.-F:GUlS_Du'&hF;;$>IPk+7qHS7@aZ`*ahY#>jqH0#q2A5#'1PY#vAno#%'Q7$UZC;$6(%<$w<rC$H_LD-<Bx=-Nsk>-oXEA-L.aL-dioi-I2Lv-p6Ms-3*ft-AZXu-[lHw-9h>9q"
"#)YuuwVi].@'+&#?<dD-l=dD-#>dD-;>dD-S]D&.m9hHMqpDD-96f=-W2oiLXuQlL$b^kP0.;hL<-F+NKh59qNc[fLPPHuuA-?;#hJ0%M9n-haWV+xt]hA1pFp#D3O[$##+ni_j%Imbi"
"QHC.^K*cL]2a&cUZk2Y4Ih6`2hfj_`D]<@j0?GIgqs)##eVEC`ZAl_Mb=sonZ.P']B_/A=s#sc<8=^i#uaP]'Z;)d*UWn$$fjbc2w=v7Id`#s$%_6L,9CP'$V*.e-UE31#'PF&#>@*LY"
"@nD##,AP##0Mc##4Yu##8f1$#<rC$#@(V$#D4i$#H@%%#LL7%#PXI%#Te[%#Xqn%#]'+&#a3=&#e?O&#iKb&#mWt&#qd0'#upB'##'U'#'3h'#+?$(#/K6(#3WH(#7dZ(#;pm(#?&*)#"
"C2<)#G>N)#KJa)#OVs)#Sc/*#WoA*#[%T*#`1g*#d=#+#hI5+#lUG+#pbY+#tnl+#x$),#&1;,#*=M,#.I`,#2Ur,#6b.-#hZhpL7PLPM0%,DN>6<VQVlh.Uf[*GVk-B`Wm9^%Xuju=Y"
"wv:YY'E7VZ)QRrZ/vNo[45k4]9],M^>.Df_CC`+`^r&MgLJWiBES4GDo$arH1</AFm;k@kV7PcDg->.NL[*Vm(V%Snt(XfC#g0]k=mk=lsd3vm_at4J(kFEX[wSiKH%h%FBZ,@'rsOxk"
"am9PJQm&v#'Gj+MNLx9)vFFrdj.+Pflx.SeS[Hi^88qCa_R5Li,L?fh;G`4o8_Cigj'c4f7jZq)e7(AO0=C]O9e$>PA3[uPCQs7RQn9R*7N1MTX`l1TP9X%b[lK]FXjp=ccb@SI`5Quc"
"2Ak--F-GY>%`4ZGK#8P#_?3L#kv1c#&eGj#I0%w#Xt:($^(w,$7$t5$cEPB$F9Tm$>)4a$L7]5%mAb&%gIq/%pmN9%p*]l%a46_%e].1&:*.w%#1<-&S*nO&sfg>&ZDrg&46RW&V++.'"
"kMTu&k$E)'q8e8'%#2xL++:GbF7#0qhh?t-]?%tZWn*O0>HkTRG>b$,M_a<HpF,nfH-Nn8+cI*apN]n/mW2[Ysxc[#(Cw<Q%GM=$<Se&,Pwqt6Y0gKMUxZh_fLD1::mvO'wSFE=`+M[u"
"vN/V.gI&w6AH;u?uKqbW`3/8oht_V-U^UR#7R<xP:p:oCXZaCOK%M7]A_FV%)%tC=c/K%PKouU[V];%lT-[u6#puO^3u7J;d<b1_ZP6cj-Hx1C;4kuHsO*Dk)&$p8sk+2UvAv+aSruiC"
"rA`caQ7DL#0)9d*L)D8S#)`&#:T-pS^DSj(BneDOwxq&#S(%KD$A>vde-2E4hx^8f-c<K;Q[f2_BSSK25E/EX.a%k1QB2Eb/7PE=MO+3_kuiK;vMedacWAw6MO=3_&=A_,RHbEO-hdWn"
"0]:kL+$/qoAn(F=(Ac?d/]Wk:H&nEkf^dwH1*j9oYGTX@S%ewZ[5###6co.Cnc(/Uqe$v#N(LS.[w$##DctM(Fn@X-3P,G4_R(f)R35N'eUKF*+W8f3pT^:/v?]s$1hUk+<uSfLwNs?#"
"HXX/2vGUV%/`5#5GNr.hr8T2-ahZY@Als>7rjT;.jL?;/fjWV7%+06&%^16&&19Q&v2BW$QDs33hS.<,U9[j<XZ*i<@HBTAZ<IF#[^0;8Y$&%usuUQ&)4KQ&/8`6&1%06&C:f:di:gl8"
"o9SP&quc'&K(^L(*I,gL^l)P('UD.3kpB:%pT)*4I,kS7dao8%M]x9&JOq7'lL[W$7%-4oR'UH29OJct=SY?62.vC5uYnU0pKT6$H4x%#vQ-iLrhV@,kx#be-W&J3h:]%'Pi#j<g;R7&"
"9<<U0(aP9O$lT#,il.moqNem%0u1RW6S/&$/Xs)#n<BoL8MdD4O,k.3-x4gLr7Bj0`55F%9mPb%Bm0I$X4;d3qJ@-dH4vr-vIn8/xQ*c40MoHdXtTfLGAZ0)3F(a4EkxY.n-:n'8LY:&"
"s^hT&=wdW0rU55(VM^#0imGn&$tk]u,2JiLX53Y/.C[E5Q@anB6O&n/K/(F*h6<?#4?drdoiJWZojRw#UrUM6.-3d3_Rd2&l_K*W_oHS^FuRE3V14#%NsBZuAoVPp&>uu#<J1D<bM9g:"
"VLA-d6C#edWHW@,?vx$Me>C:%N]>02Vu]>,Ek[s$3^>g)&.wW_i(UgMSF^I*$]K+*H&X/M=1,S.nh+D#g+)ZYv?]s$Ivnb4OD,c4&udY#>as%$*S:e6_+WA#nUH&HoMr&$+ph_SLr=:&"
"u$H]un3=@I9(*Zut/.7$$PxA#K:-,6&][x*RYY6/_U?QD.@lp%^6PM4EA*5vbk,e8.Lk5&4wZj'couv@JKtV&^7f@7e64,2pcjwY&(_l'j<RIqY;7YlH16uYa8)w.1akW6.R###F/@8#"
"Yj8I$?<r$#xD-(#31SnL(2^I*f@Yv$W4Qth)RL,3U4d##;qw6/+r9C47'$gLCAE.3@_#V/FU1DNsiK,N=#Jd)O06GM>nD*.u>tB9%Bfw,sPjs&<-:>G,%%gu8,3o&_?,HVcxpv)31-t0"
"]9O)%@4T%QVHQ-/.p8t&sL@a=irDvHLdr5GIr.MN/prP*7/d_/c:#:eiMEo'&le@#o$Vxtlq5_.B-*=0&PH1GDoMxJ6>4/,S*E+.<U@n0m@N$#T++,M%i+Ds^sJM0ePB3kuMNhYus-)*"
")dE.3%P'f)_.4n-ucn$p/Ug:/xQ*c4qBbKl[1*C&DM[v$oH%x,=*GdDU?]s$WQ?&8OlB9A]^6u%*`bg1D/0i)P_QMUrRt0$fS)i1%)?f=[01SW5,IjrEHUw#_3KV@PE=?-:x9t8jU6k'"
"'m'1Cbrxo^V_j8.d'fr1q5J&/wCZKDfx1Zuj0q(-O/TP/g_N9`iiMxbJ2exXK'kl&74*X-H:RgNQgaR8d&AR34pb%FanD5SGL/R3p&uK)sM[L($V'f)a#uKlcm;hL/8UB#oK(V&P$tMF"
"%@u:&lv=@c4pBa8g-W/ufi+r&QemQ8r-O($ZwcP)n2I&#TUP#v+NJ]$V%<fM/LihLsg>9%0F7lL*veU/)QOZ6_<*v#X`.h+Xha?%QVr=Kh?cv#v'DG[h$]104T;88&KK'PwkMA0i5(Q&"
"DduH?sn^4fK59M^XE^`3j/Bm8>ei-*]3B:%]S$w-%Fo8%3X<N0NAg+4CY@C#o'q+MC]Qk%./^C4WCH;-HUd:A/lQT.I.(]$/'(Q&/CG**Ax<B4`6Tw#?^9,5ISl^,Dm0,+^WBH21)iE6"
"4:uk'%wl,,7^'97DP+*$OYG>@k[E89/.3-$d6Z'#C5I8#'J1;$21JS8]$6K)mAqB#F?+W-<c1LPliA+4?:v7$<k.5(Yi&1VJ`ot%V*CU&]rZv#guqFXW7vM9L8`97jO0'$PO4t;x1B(#"
"+kJE#Vfa1#Trgo.@uh8.Eo3ed9cJS@9<<U02jiR:8mb@9-erQNiX:`sf<<>GAq#I6iX1f)>VZ8/r'l%$SLtk'A^=E;bB#53Pr;-2eAUw0R(IlSt3r7RpsQs./oSfL40)Q2j)mb<ouuH;"
"luoMDrb+fEDbV.0C7YY#q[g2#tbK`$Sq78%2g(T/)]28I0+9,/AL*?#OtbSRe*@^tFU5O'hjg_,m)l5/^4/5#093jL[ndG*FF=K1oUFb3=sdJ(&i^F*/gSW&@[4?.]l`iK0):%0CQL.,"
"ui>B#rYH&W7':`CoNsu-Z%fYYX04,u&rd'-</5%$bG:x%9mhILhgl9%w0lx#-OL$$Qu4wg0O2g`3jHG)6^gN(.?Tv--N$?$3/s20?\?Ea4bf*05JV$40rnaOo+1c:&xS6t-n*j$:Y*qv%"
"jN&9X%JTb4KCkw93SJ]$,*DiMwOGA#E4n8%;Q:a3e.^C4=]U@,o29Z-@g(T/?D,c4XSYS.sf6<.bNx>-^E=g$l>H`#4ap%O*AGmUBuNq.BGTUWRd6_u*_Ht_mW,nClHIY;^/D,PgeMQ&"
"+N3WFesJ,8:^Qjuc-6*.me5+'ddt,2v0#p@jB:?8R(.cQ6w]^/[Bd0(^J8*3V]6t08WK^cOO2m/XIGP8HPlG3`cld$ur(K)oSng<(eX(0BfWF3U77]6lei*/CT(T/S<opKFVOjL9_vxL"
"'q7^/=33E4NuY?#=,$DFOs<d9ddb(/Lb,s6-h`Pg_)X'.Er5n'xDpE.jfgr.P(mS&AYfx$FB(]$.nB(5;+WTC(F%&;WT#T9T`JE3,FU$$FJA?lPq)t6+W%CoqG/s?c7=c61B[d9.5YY#"
"B-*##Vr?/UO8[`*CCj,*(rF,MNo@X-`H,T%r9%J372tM(`?=%7Fird2&U<d;eX`uPbJVZuBc6gu1WgB'aJ)76e+P#%x`H/;)&2UOCxBJ#3riuLf*)SeYTB)3HqMP/PegZ-l5MG)(n@X-"
"^>>W-@ON'6)&d9%dR;Z-qh'C4tjc.qJGe:?IwQ`EcsrPGM'1.77fdrHLW1Kq]c#SRfuNDWC`7e2uR`rpGxM:m6xN)5K2F^O6.-5R@0q)3;n=)#uX9^#AD2_Jc]C5SwP:D3un0w-VM[L("
"sJ))37&Us-hcXjL$_+c4@,+gL,t&8%kiY$0S3KPJhuYY7isaEB&[5p3mH&=]B3sYO>/b?]a6l2(70=tmKeE,B'10B'?u+u%wRA[uxuh<9Jw3JGWU:d6.&C5OwE71g@+>v/Uv;+*e;w^."
"Sdpn)mTmB#@'*##Lg>,V[E]?Tn^D.3,&1hPa4d##GX4T%u<7f3[>:Z-N=+D#.C5XIF,ej(R@Lh/<X_n'kl&_+(6@q/8Y7,V9]A2Bleb`Gn[%n)q7`v#R6i$)-Q3_Aa]m@b3mcK)3>]]+"
"8.gr6-5b>$*Q:-*cQbX$GVkfMe+#o.gIKF*,dA7.b7m683fB#$6Il;H,K+P(mDUv-M6sxFI>&w%FdqXlBL,onGRiCA;p?7GcsHXRK1T(03ubjHV;$B$Q8]Yu8Sgi@-@9''=VJ[uIFS2;"
"O`/9VHk(x1,?jtCL6rE#jecr&AhE>+Y)9q'.;6@-owfau>cCt%Z%S@#jI`1C_wnf$$NmZ$xekP9e_jDYX'xf$qU-1Md4Bj0:)Ml-$A<Sa^g%?>oT.BNoglqL>XI#7hg<.Ap2e%(_h>--"
"^0(-kk.$r/lN#2gL6Bt[-h^PSj%p(uGH]#/BJDH2&fL=-Rs;9TYUp+Mg`)*sag)/3]I`@#kRYw$nRbW8V+/8@`lUP&q,U99Ms849/dqKa[Pmv$UekD#nL[w#_YKp7^?1$5$TP9%iWtZ#"
"5w^G=[Lb7:fv>q9xV,<-qAIU0F]7H#m8A5#764l9Zg5w-t`k1)^b%s$&@xf$rx&J3jik/:s5In&iE1'-.L'X8jj.&(Zl+;&h;EEu]sm0)i,,j'IOq:#LlE9#H*Fx#_ui[%FVK+*RD+G4"
"0_#V/P-Tu.uf6<.Sq%s$u'rg&JSpOLleXY5@_G;@5nvJ#7.bk'PJKL#n;/[u]YUfLNbP',?==f2#v)F=R28k5CGJfLiQSS#w;-_#Gx2FM5ql*8w#:<Jutq_%S`M^#=T1$5v8Lp9M*7u&"
"A$lO0QITV696v;6NuH01tQ*ZuL6P>%.PE_&#m]E[odxr$CvAM^Qij)<U:d##aU)Q/?\?Ea4X3NZ6_Pj.3huJ+*dL4GM@o*L(8`hJ#6?.AuJg*/:MNh]4:-;fqOt4`uoBY>#$91lq>lqo."
"X7'$5<naCa)`JfLQ_6##EI:;$);d3#p%eR936q#$W5MG)Z%c4%B4'E#17w29)QK/)'TUe-A@:f3s]XI#hjL/VeS$Y%&WvL8k#@)QWXm5&=MSx$xmN/4*Uqt%LkRH[gJf)41J`=c+mRq&"
"JJFm'.wS`Ew?4E.]nOu$VMpg<?Y]^tm4[n9%nVQ19LXwTBnc+M?kbA4*-v1B/f@h)rh,K)^]?<.jw<K;biUE4*%C(4oJUv-7U.:.xQ*c4FJm,*uIKF*3`BgLouR)*BI@lL1-cX$hpOp."
"/33E48jJb.-Haa4<@w`43rr58_SG8%V`GUIm:o'$+P#>5p(4'H.D6U*%>ms6XTLo95`$f2?S-E/IMGB#oq`fDYpFu%v>7duZQ>A$q6hK,dPufLt/FLWEE_A4hHsEu@n$j9X[Y;)K0Cv1"
"5<38AO?W:':PVN=8j8(5M:s;%x8IlfN.5v0gCT*W6(pL;9vwr<[53q,P)YU1o-j04.RL7'u*5gL6K7S'M@sM0CI:;$G;d3#UDHj:ib5<.aQv)4sJV@.__R'MJE%f3_S$w-Jqv%F9j(#1"
"JA3DuJ$E[$wY:2'm5gcDMI-=-Z^nX$H9lA?Tn9($b`_]=OD,nLU,#d;5C([uAKNL#7I:$$JlsEIg=f:d6APmTY7PV-&IML3S/pk9+YpZ-kUFb3NO@X-X]j/%'9dJ(Jc.T%MY<9/)OU5i"
"5g;HcJ/D'Qh#Oe4GtrMgJAD))[WXP/;hUJhvRwiT,Tc>d+4;2BAYoM@C&d5&:u-guJ`D[&PYR1+Q.tTufN3l'/c<g5.),##s.b+.1=lt7p,OB,Hnr?#pT)*4'->g)kSnY%L@qR1?wvM9"
"-M>s.Wd#X$FAjk'T5gXuufl#DsB<a5@_`#5LTH>%nG4Annbki0DQd^T8$KE50=JMD.MmfC2@6xOwZsgC^4o'7&08#<U-G99M]*];1/5##08@uLjlgi'N+lKP-t/3MAg*P(`bp&$hlQC#"
"?q77jp%9HB4nj%;#fmi0:?RGiT3#jT-S5)u_WFv/3BX204*iOAu.Sw##<jt%/.c%O/oBgM%Mi8.r9]s$5KMA.u_:W-0UY-H2SD^mv*Ta#G#Ar2hc6_u@iB95fq`$:k+KY]qm<GDR:Tau"
"%)I`AT&W:0@C.^6n;iZu1xvQ'/:@+5R]vc2P<Kp/FJ7h8ELuG-sHk3.,JUTNb(XbNUU..MpwgN(@f)T/o23E4x_0AO@XAn/Xi/igT,r+VQg$CuDRi.3Xi^-4^Eu&$pCQt0J<>Du[@7B$"
"))T$%JUr;-IHvD-^[uh%W%`8.T@vr-o&1L)p1pb4dbZV-;vjI3c*/N(@41gacXme43$?o)0Oc3($:M:oP_i&%2OrhhJM4rdbeGPAo/NhB.ZRr2D($jT]P;SKHv'RBO;kMKZZWL^j+M0,"
"q,QE3>=G%Xvb5VR%4-,MU;'XLvdoud1YA_,45&&+jc;F3IO@X-@p*gLh4Q)4?+1H2DRu)4&gm+)cVNPA>5_n/THGDuKk]8'RFjCu`k<L<S%C-0f.rf1`a-n':jpJ1(/+R-pvG`-&c;-m"
"0g_$'-CsJ)xD[q0;Xbg1^2I^$nn[hgRTr>$We-LWTx`60@+</U@%$##oa;wIoZ,e<M0x;65tFA#0/WF3MD^+4N$FYl=7ZCjJcS#mWDwA6W8U^#VRQ[$KC1bt@?$u&5u3]/pG?OCmISG;"
"ZOH29eXp_-KPgQjfXocVE@Aw[M$L,3SdDb3L$3XC8O_hL7wul$bf6<.j>#n%*,Fb3K&+<6-flP?N9)X6BRvd2f?oV4Xm,Q6C5a[$P=:^uD`u?#;F0w#0c@lJ>`>5AM'FW6$sPWGZluI:"
"P$)_$SmZgL-R<igj0M21PNO/):&;H*NKR,*OYFZ5C'Ch1sH3A6dRjP:Y;&GD&MnW$KPC=Y<Fd,Mj7'I(qE4`6_'>80H?*##rb%jTnxep0`QXG%#BqB#ljYY;nd5g):xc<-6*lg$u?]s$"
"je#B=Z.L>,f1iDIGH3tE7Ga%$c2`M4)F-?%ICKv5:RZ=RQTM#5k[VP6SYTfG9iPdtmn3C$OGu1)%GW_t_4(n/Yu5v0iVvS),vZ$;TQ*j9mdgIM*t1e$PDxx+%S:aNA-%?$lJIuLo0#n2"
"qkn<%*Vp@men^Lp,Gj%$emja$D<(^?,pp3=1)m72hZ7kti-IK1rW_-@edpdOL%1kLGNs$#mD[v8o).m0]vu9Kii=c4wV/gL]wAa#QsEo3?#s9,`*Q2qVQ1C9OR**2[$@a+^CUW$_ZukL"
"<B2N5]KXj2?jVH@*Rm;uAQu1)rK0n&v$I;1Iq`0:)1sEIxuou,_xwY-.Ad8.s29Z-%qs;-(Kxn$bx+P(sf6<.+IDJ:KRNr(^b@UBC$TH2:U'NK/A/j8e>)2;'),G1'?>3@2oBx5oO@g:"
".@1(&*Gx@#;*a2)>c2]b8c[A4pDvA,twVWfjGC:%hFs(=qPY8/Hl8g%ac230iu@A4[m5%-pnrsf=)Sd#05CLL5qCm4J*XLD8B<#&c'Qh,$&&<ki-Uc30mQH2c'RLM[;-onw)#t'Yn(t3"
"7jt3FQuHD(qElT@VdTS4f]c&Sm$.lf-iUv0&m'N&(ZrY]:gTG;RBn4*KqWC&:b&/1TuFgLDcWR8@QY8/bC`+8ExJ,3kE'd%/2VM'aXec2<$R^TdoOE*wKQ4qFrn]4Vl,v#?hGc6JMP/3"
"&;_w3C<n`A7dWfu5`Rm8Z0G%.(FZu6ob[g:0=>J$2=3>7,Ys_7l#L-%vewiTxI=Q1oHnm%mf6<.=Fn8%l1^F*jpB:%o*&T.xQ*c4DwO,%.doF4urZW.GL4T%a7wK#1Q:v$b=?DcpMi@4"
"NQkA#KLflG<Q_4,v8o'/MxXU&YiuD#&*A)XpE4kG@g$jTM6_t@^#PUA/07n&b+=TBD_(pE/:j^?MjCa>ht+jDrBBw#jm&B,(t0tN@-8BXZ'U6W.J-<-*o^w%Mt+T%nKv)4bV1Z#<2Vau"
"rcHq&<.ls&X];*3`w_]tSC^v5KKCB4xw>1%)V+wp1Z_dO*ggW$nh'C4[Vaj$(8Sv-W.]w'jGmF)Zl(v#/uv#&H(Ob366C&.#$vn'LxvbtTES#HI4hP82bo`t+`qc,4uUA$A-RF@UA&&+"
"O:7.3t='+*'TXl(alK.*G4[s$@3d`-OWuQWF1Pd=H7f7V&c),+v$$P;?lbc9&'<n'?tHJu%ESvL8KNh#r'2-2@]XjYq[h`<w,]1Mn]o8#4]h;-RQ;a-`hwh*^Ou2<T&GnEirr$>s/j*e"
"WKpC#g((F.$Lmx8V/0cXDJY@PK96*3e*x<7XDflEgi>$E54bE&>kM2I,wp3*s0pl:p@db`:pnlJsO%$73#fhgr59tHOJE`,IZkQ9#:6?$6)H3#FqKDM+U[#%BHg?TSF57_D6Zl$)J+gL"
"&H5gL>@iS(?bg:/IDnC#3_x_@f*gs'#&a&T<s^n/Owj40d<*$&J(,q&O3`MTVG)f4ZYW1+,/]+0jV$_uxqr)?Z00d4w^>oF3^Vl*s,Ua6(PLi^J2J>>E5:-00RLs.oq]9^loBg-7Wi<L"
"?i,Q8f0Bv-]p^b%F`4Y%q4S`c$2K&$($q(PBS`d2E9d>L^95Eu3uWr&r-u]#-g:E<#Z_#;#px:-%A$Gi;T0g#nmAKW'YnZ$rx%88*ckA#[->w$ZekD#wFKV6%5Hb%?_+D#Nk457%=#j#"
"EMvs6?/%dFQVsICj&ZMTCn,j+7PP0+LuOU&?='M+fjeH%'Ms%$a`aj$v**##wbdPS0J###nDK%#]3B:%CL]0>A@E;$*Su+DU*bZubFi,@:_es$#;UF$CTEm/2WcvdV+Y;-E^Z`*w'Es-"
"(97.MQL[P/mm+g$$C2E3R<&)EDOoC#.e):0q.$I*VT=S/],kj9x9Ub*NILJuZ_o?5b0/;Hj[/J1%=T=HCWY7&.Jt'#1wk2vf+:+v2h1$#%a-Y8RF*T/2Y?:$Aj.+ue-AB=Rk5W6SIjL1"
"q+r58NLeS#5;U'#0,>>#XpVa4;(]%-Gq[s$K3pG<2)o8%-sY8RF&k%tbx<M.H.]u.)jNK)%/5##JI:;$b]w8#v3:+=dR5r^KiPd$FlbU0[Is$>KB6$$$]K+*t?(E#q*$B=+N;;6u+F4o"
"cW6sOi/xrC4^Ws;0]b:9>j9.lvO>rfhA,T4?r,)$9o)./YWZNUm>w^7I`?5<<NR]8FfR0>ULIURgq?1gGG1t%4c%R5DJR6M^),9#Q;QZ$s4=1:g5VC$M5U&=IN(E4[(KL(9DHE4,K+P("
"iM+jLk`&q'+'F@q/$'&ED[P-L)s;[$DhBs8sPNs-$=@AC@FB''#POE==+;au1n$d,Tw/D$;O%>uB;^I=:H(WHIsJi^6Ir)-hLsJ1F@uu#jN]5#M,>>#B<9;K#KTW$2pg;-uk6a.)gK.*"
"X#]/&@4W^#cFwB/>,vG2D9Xh*(pJQSc_jt4KhcW6+],Adl-hf1Yeq)8w1#d=5qK7)=`b9:93g#G]n0-2T+d6JIs+>PV4'kt9&#ktQ#cSRgpIkto/la4+g^I*Kk4D#W2YW-?'sjtUa';m"
"ZU@l<bkKTofjgUNw_T'[v+Z5?mA^+(SCUW$<]f`t+J1g+1/#D#AV:g+*LJ_2]MQ_uN)To'u:A@#$*axG%dA7.mYB%M'd<8$+VJGN`JGA#hJ9s7#[La4qJ))3Z]Qc%v(G)4sf6<.1<##>"
"M50T8nJNi/5,)%7fo]8[$f9-LXmhMXH)cc8ljP$$A'MA6$&0PFuc-K>lY,52vNvA#4UL23kL.9<FoOgLC@+Vd0fCDbFx0gLThXI)b<Fg)B)MA7?A5d$ngBu.2'mk*4-4bH?Ucc2Vd6*3"
"[*i,4Ps;u/^amr&Jid#-Aqc7'@^sV3V+E)3Fj[##D5RouRKIw#dl9'#7Igw'src8.(cK+*8V-W-d[EM*9H&(%DLVp@10(:qqCF<>WW`=O5obq&=pG,E6,?T>UmU3XSbF@m9Ct:;IR/W'"
"KG9[uxmleSZSm@=:]_OT(;k#5K<4H*4$;wG'Aav7lrDN>598tuVYHf*d7A@#aRZw$`3n0#lRb7eEJxY,=o/Z-V,M]$7&'J3^[7W-rxY1WPP+o$O%A['m^aV$?6HGMmEU;AYuq]Zjtsc`"
"_;m]6S;HH2P=:u&$)T@utf]<?q7_e4)]C=-(cxm';s5g1L5YY#t18_AN122^0^(;?W2mg)H-*Z68PSC>Ag+REM'(11jl*_$@4OW8dbag7V$[3B'ET0'9WeB1DmxY#8`)#MR#anud*9Q$"
"g&kB-5kWr$4mR8/fMmw9*BqB#)KjI3OV>c4=<Na3I>N=(2r[v7mM+acaQEc3LU&gHu8m]4OhM&6Dq+k0DoVj&BS=dP;[u89jT&DXMC;f6ICHX0IY*9#Q@gm$(cxs7,wsY-H(Z5_jZ@S;"
"#L(a4Kb37%Xb`v5mfi>5+:D&9s0h3(Un'wKJH6>%`5k],q8t]6S>Zd2MLXi+j-E`5mrDd>(se_6YTV^4uQ2x$g#%7'leYGM&0Qf_H@OfLil)/:f$Zg)xnh8.daim$:Y0DlGk)'4>*jr'"
"U;$h1v4Cr1fxR-)SK_k-+R1;?b'Tv.%U5d)3o(Xom>C:%x&gW-Au7@'_S$w-q$cGMeLZe4]x*B*.8*$JS0j;'$>td,@m;=%<@5=(58W9&@?aD1gK-4E>x5AuTg6R&xRiFR%w=;HR+9iB"
"id-_#)o1$$2TEs8h)euYMuF$#Uen8.P%ESn's,W-9p]32I*w]/,9Hj'PdB:%TS8K1ugNJ:o=:*.Mh,w5[B?0hIbr-*P,oZ5E2bL6TAVv:F/0;Q%.1Q&vt=rHU8%@'M.lt<15YY#D3*##"
"<pKYlW7lr-C?+<-1AvT%RIb%$<YE+3Ba?T%=Fo8%4DLmI+Km1.[@mKJA%M/:QuEc*s<82pJEq%$*0kr8R7S18AX*;84wlt<vjVsu[9U$$our?#w[HW%dr;Y0NDqN$p1b9#Xl;0;Fi8vf"
"I/t'%$*l.HB^-T%8JhJ#o6%,s/NITSu#8&4-e*-;.[-=-lRmAbWhXEag?8Jh;,5`u,F5/+Hk-Y2(Jgo[J(:[u1_$&I/grl*;lf9==BZ4*#lhl/A#7ouU2Ox#=sd'8^UrRs=TQT%g4vr-"
"PQouG&AI<%sRiiMKbDxNTT_r?0hZ`5C-PJq-<%peUC$5fKb@+u_S&*3^D3=SOT[uY2I:[u[]Eh<9^mlJ=Ji+V4q#vGp?Fm/O@uu#R:@8#/)WF&:o)69hai6NE4Fb3jr@NiO02h1^0,T^"
"#+m5&?S]x$nj,;dt`iH4.n6p)InYI6S0nmiIfW^-ld/R<C'<Jhvk-(dMc$w-'Cs?#8XQqT@`)T/ej>j0l5MG)YK[+41`NT/cTj5AOJ,G4[L$$Z>7o9.pP;J%&+>&$GBWfJ'.Kx@0jTs'"
"JUgV6,_/7$&[>nFmj3+$o'OO11[o9.rYD#$wNZ*=3DPR:_obVR7.S3:&Dkm1^;D$>J6sc>NYgP1O4OmLSAk=c3Zo._;iXY,J8(W-:LEf4S;,l9nn(9/pFxU/=c%T%R2tS73vhT9eVc*/"
"WI9300hN(/#>7,VYNcf$D4@>u]w]o'Nd3Z62jf_#p';0(c/IY2ERb7e<SQ`aVFb`*Gjc-*uIKF**W=P()g^I*<t^T%<&+<6&SYS5(7X^ksHf4IgooI&MrM.Hw'eg#v=BCuR(gi*U60d#"
"&(gpgTvirZ_kBA+IOV,3ON>3Dsc$i$DUnG;HK)B4^omC#i.Ma<3Do1Eqx^i)6<IH3(DRrBg7dD59h&2@t?U/6LQD/(0N_^uZ/2tGkl*R#CX#B;rr]E[QT.)$#_]L;))>NDx)-M;OpcG*"
"cJ5W-.5@bY/CuY%(gac3f=fP/MrpoK/KiL5`xVw$kIQ85jF%lF/CVfV5$9Jh<<@xO&]8w#eP`Z#AePt$6FK+MpU/<$fEXnKT-_H2K]WP>UT3`R-gCPS,$B#dYX^`N?_Q21E>1H3+]Fb3"
"R*B6/oUFb31`nJ;Q+,d3KX=U%bN.v5?TRih[6*v@qxTj(@L-9TQ(qj(gjT#Ax+$n/dA^g*(XgE#,Zqm&a(4-9=9.bu``+U.A(I%-/H$Y4t):Q9@N@F##f?>#[m^=c/CEYlXHlX]cSob4"
"wh?&/`4pL(KBJ^dVRDe$KeTau0OYh%-d#p%eVxl>+LV&%J=[Yuq+7bu[HjQ&CrSX%Q]NpSoCA$T<MTf6Mf7Eu5E#p'iP;Z#1uEN9$B-d9,(C`1AHE%Ip@(E+h;_$9.W%R3vZjabYX^`N"
"e'CA+,<t-;vOvv$qt'f)Ie1m;+++D#<r0H2YqDJ4NA>V-tpm8'@x?N6dVl(N.H;($n7FG4aiZAu[Lg@6t>:j*)X^+i=/CR/%(IM0B/@8#oHu^$kpPQ8g9kM(n]N&4f.*p%ET2^4sD^W$"
";&+:mVM&]$.vb*.Wm[d#FDM0$'/LhL)'&i)x,Tv-?;Rv$IBuD#'cXF3*SeiTNT9A6D`MCGZI0j4fbQf3MHL:U8W6-5<5Hh3bOf-H7kq^fNd*sZox8_8Hj#Q/xIkI)<0fX-CL+T%$Rob4"
"DF(a4?_#V/tKZ)4;W8f3sJ))3%i:8.+E/H:<_B'@YpvC#K*Sa'PhWO#Oxvi2fQZC#vq;O')+$Js^pK-kUYpP8Dl0^RS`FOO0n<;7lkx^M-u>Rp2i:B-&qAPF)`JJ)YD[Y#@IDG;Iq@)<"
"8's+;5]GwK82pb4c`8GMd#NT/Wuk;%)c:5/$]K+*CEBiD*%vG*v2Q6W+?jv$pH3Q/[7*T.I$4Q/8J8r/OJ,G4;Cq^5Au-20k*`N(odSn/oh8@'H4aT/W^(0(PA5%I5k(O12<IP1)FE,-"
"^)`K()>5%I3O5n0eh':)J/S2(Nh@[#?KCU%s9jfN[<cp%iAf^5B$s%,YhPwHo#A2(gFE>.9&3s1=LW&5FeJ4MFdD0(Tqnq.2.P316'fTifV1vuq:uU$o(02#NTdd3cVhv->;gF4kG_Z-"
"wH`Boki5-3RJ))3ig'u$kv+T.,R7x,d;Jj/KG>c4Co%&4IH2hl7jE.3j>ojL$4_F*LCJZ$w+YV-&$;Z-lkgDNpJ(u$$S0+*7RL`N(XpoL.h+G4mY$HD2pB8.*8>YuimY3'+g+0J>S%4'"
"t&CD3tcW$Bb79+*FtDB?4.QG*al<a*r&ViU6gt^,C.)?#_,IH)AAe.2S*0m&9mFi,0Iuj'fShq%n#CW$vCsp'QRiZuK#He#Ephk'-[php$ZqV%-1sW)A+&'-MDZx,lSvr%K_BL(o+wE)"
"b$92'KQ.<ZTW#R&:TSf&^Sr0(XQ^m&]`Ih(.2[C)J8;6&/oT?,VHOX$@W?)*I@@8%Qf]#,=kxi'hU?5fTW,R&0DwP&VZ#R&IZ<qG3r*=$eE9U%KE,n&;]1v#?_)n8%5YY#-x/PAKP(N0"
"-l68%WKBD3@6;hLJEZuYndLP/+inko5vqkoNHV'#0.=*$#MS5#aaFoL5)>c4?_#V/loXjL9DUv-f14.)[B)t.g<7f3Dt>V/kM]^-YA=KM>r,Z,TpB:%nE[,X8hTx$BKc8/mGpkL;_dV-"
"$b-F#?T?)*K,'0<:X*;:Uq7fJ;j%3DfG&W6c:fp%_JxX3_w^K<5OCH#Wk,VCwA5r1+T(?T$=14+;'UE#0*^s7K%#N'xp=E,>$O/u:L.Z73qSd7bV5g?ct-IMiL^@%wMQCb0S=@$)Jh3O"
"qEH+Za+Q%5tY<+ZSt]>#g#fM1I8w6gbaC)*C=+,)JC$##,2'J36PDX'dZZ;)3=i9':k`39:,gQ(;?FH;7gEgPx5Gp%$1fK:WglN'7T`Z>T^P3'$4xg:6.r5/9^%w>t*lgL4BW%$wh/ip"
"ZA]S[jgx+2(3&##rLfD*u7XI)6+r;$_o;T/`?7f3rT9Y.Hghc)-U%],k(TF4UKKW$KF;8.ootr-g<7f3&kG11DfWF3,#hI*e.<9/xS-=.YiWI)M._>cD`E+3,a0i)3(;/:,)[HD,8q4E"
"l([3SngBCE*j'SDE2iZuXFIqCbqq^NfkC<B]ITvA'wDOtEK%VrKq2bMkqfZ$lW/N0qWdI<rk'[>g$2^#G5a8:kro44bXiPG4g&ip;l?+r(5)iFVsZP&]iXM'dW+fuW>e$Od_(wA[_GWB"
"%qDkt[Wn6Dfp5Co=:Z&N^c]Y,+7PD+w(r(@-M9rIMhr^#mHBq1w/5##9jCC$eh,3#e,_'#n#]I*h>.H)SLxnhhoOZ6qI,G42*E.3[k[s$Vi&f)W=FT%e8^F4=93m(`cD>,x+,8r`J/B#"
"w1Kq'AJuG#&=%U1O+6a^2KKPSaPCBY+oA.q_l^6(vmkG*&U>n@swAVBHcam'Ix'Z`6.@vZ>-/bIPJdu>/c%,V+]$s$WkQX-pO0N(<s=/:J6Zx+96q(*XZN1#$SS2`#Bt$#XbY+#8->>#"
"#i^F*_bpr6rC^+4YlR_#@GI['h_G)4G&9f3GT(T/3xWD#BsBN(d5%&4)tB-%)gC/s3^H_#N9wX-+]Pv,)V`?#cTk&-C<Z33nN1]1Q4GU0Pu[_u^*es.Ip)_7tNBZ7kA+.1SmA,;BGe#5"
"FCH29,qi7'cG+&/omHJPfEW@,SI60V%0ttITp7Z9_jtvQU@J/$n#:J1c5YY#.lvrH557)WEj?D*F]tr-iL0+*I5^+4?G269>_`x->;gF4I(HYJpvT]I_Bv4WQ?RSVVUU5M9_S,?4SMKW"
"Pj4:WYT?4#<mQO$J04.%wXbgLiL7.'4_)Ojn@tA+<e/<-JY260+<&##iL%jTlx_$'9%1N(N93j1#[w`%w*^cVESP4>(<gK]Zt8Ht$agItgr[l%,k&qaP`.JL/+APJ*M*,)2@@:7$K+O'"
"ISBu&I4&r%McT>&mMOV'QRo3'YqeP)[nS3':S4B#Aw**$K%T&#?of^$AbqT/@Dn;%v%xA4<MVo%8L;GD7+8E5vMb5(u)Qv.n>huAhi2l<a.Ao0KY/1(B$IF$a*(O;G/$##?mq7#;CLt%"
"Q%vqAO+aw$VRI@#YD@d)2ia@$kXV=-<i__%TjWI)H4n8%:R1x5cKNu-OaW6I+FD>,RBVJ(SIUZ%P2MQ#xTvNCFY<t?<SV0:KkkEMpAIYm$#gB-<^rdCim;k(F;A>c7uX>-H-A?>I#WFr"
"tMd33hphm6rLMO#HR@D$Uf1$#R[2Q/%a5J*:d*E*uvQ_sX1Wh#xM$qLaQ%)EBh_$'^5>##k$/9$wP]5#?_R%#c$(,)P%1f)?l)r(3Ioq.q7pE#sM@^HE>#[P6e:DN,6'xps@)X-4a?>("
"4?%B#q97w^<*#)a4V###<;SC',l>c4YbS6Cpj7n1[nuEk^iwnZ6-hP#=2b69@*,##?Cj%$;)H3#DmV]+8m4:.nKSknQ&UunI13D#Q$cWioSx>-+E6T/c9$`4Z$5)k(.f^5xo^MTm,tIU"
"[3`_u-L%Z#Sbvu#lur'4Zrcu#TL,DKa1x:.U8kCjs7]59#OLk+jv-b#7UYH3x,Tv-j1uO(:]%fGfq@UR`KZg$9NZW8]&^OO%2G>#8a6g#Jer,#P?#h1x]DD36*YA#f#`O'dU4GMdij/2"
"3>%&45U(AX.ua9%.F;j91YC;$/5mT%A_[<$')@s$*'V$%xJAL;Dtap%7dWX&5jCv>@Ues$RO>'#vN<D<iux4]SV`&d7w1M%>;Rv$mH7lLLqAc$@cQiTAWx^5AW4$6mq1O;3ocn:L*9g0"
"?:9q/cn40;1ll3;vwH-Zls-PAuvTBo[]###Bfgl8<@(a4=46xL&K(a4<siK<@a+$6:+W)F'Q##Jr1ki4v]lY-SSrp9*rcn4]s7J0)%v`EeT)gCt:`$'..ji0>x;9/wtX;.,KP,*O_+T%"
"XE+m/@CYC$]xBN2c+OB4m#8o10+G@$1*,X$o=9O*W<9.Eg,gF*K4*R<w$$Pf0sT8%39pl&3Rrv@E_6m8wW9/DQpOW-m21`&7@a(9#qR5'pIR6/Yx[L$HnVr7qpq&5ZG2DW_Q:hGK),##"
"%Fte%=HFV.JH7g)?I,hLgL-##0ffX#6rjjL.`X&#LNv)4+d0i)[+KF*O0d51CUUQ:Y)J`sV^V[o/QZGIA5Pw9mDq92lJ6:2G`#'?eioF4@6**4=PL[-a*Fu1O6v]u,=UJ#_'D^R8p*&+"
"^ar^#?[EDaPK/Q/4j%W?_4$Z$l-S<-Q(8f&eEEl?l^Qfr#RJ<->1Q<-#lhl/I`jP$9Bl#%p]fH-Z9Z&HAo2JU4CHv$MDlD%]1HpKVL6##/YMs#5sv5.tkYxOB]^s7--lA#wSCx$=;C<-"
"PK%R&R),##`Ho9$w<d3#V,>>#:%Wp.jpB:%wPV]fLiUv-XH3-*cxK8.@d<c4YCh8.:<1T%RbL+*AUWg1pkVD521ht%Vxb-,Jg7P1`[*>AWc9(%x['rCbw7oL'Glc4.T>I>`c66MKuP<-"
"pUUl1C=lduV/lu#X1w`C:RpKMN'[l1U/bf-2,?wTgjBwTL6&##^4[r;=8P)4ZKb+RTYMT/cwY8/&B+4%(ba#%>6/H>f0Ab4hG5MCbr[g1q'8A60.LX%akHF-DqelLd5CuA@&aFeFn]7s"
"rsDe&kC_)=IOe'5OF?N1i<w1Z@NhS-?hYe)+@3MZUxSKiJ</m/'lVP&%R`n8ImZp.Kj+BJwo=^Jr_4gJs`xAJ/N%v#K5=xtZG(p%^hHP/_*%##dBww%pd2/)U(r<-$x7$(5R]`X-]d<-"
"b5Ck0(LLrZ=;%g(;778%fkCD#,wC,)aZpP##c8/fiq,V#nASCs>B''#VZ(NCIW:Vd/(ja4<Q#x#U)huGMFr^>YJ=5&Rpt]uIA=_/^(S1vdZbi$j6>##]ZBD3&@42$xs@u>KL&Tt:3H50"
"S_$##F1[MT?I;kF&EsI3krjA4QY6<.qL]L(`3lD#k_Y)4WCuM(.)@j:Ul`#5^QX@D,HP;Dh;#hudkXcDPr74C?]XUbeDB1gQHAJC*RFhoJ1kQ#C%:I7CDCM6/`oS6g1C.3rQP)'%4K@["
"<q6g).DXI)7Fp,&LOTfLuV#h%(bC:^(slA#C&0''<d7duO8V*'5Eu1Q6uHLGLSo9=$:d,E]Hg],#@Ve&QV;&->ime>i/`,2V3=G2sKZ8[/pDD*#Yr4Jxf#N(+'>,%Ior20?f6<.q%:8."
"8IPn*>_4S9aM:a4hsdI<c);t.?@i?#ax;9/lf&bGKJXJM3JbF3U$nO(7q<iMh3CkLt2/n$lUPv$f4:lLu@i8.kA,n%Zpp1TA/cT8Udup%fM',*n8->8SmUQ&$_x/.B+6k2kQGS9R^/&6"
"19=e>U`=>%C/$'CuG1a+Hf.T%5]rd#I5RA@?q=5&23*v#u:JA4mj[PJ:Y2Xo^rFoC'Bv',*ol$,*81_%:AfHuZ`CgC6L]S%wi[]@?dd%fq7-Yu?Gg.=/on`aTCf31FSEoCLAsJ>NORJC"
"^i#b7%t-lf`8<UoO>(B3i_=&HvvaiB)'F@#4u$F8j;nfENICE@bDYc#<uqV$jr(8[NQiIhRk.j0:EIP8d-LhLCr/J3<I'>.PJ>c483E.3WOT&4A4n5/xa)*4@Dn;%?1[s$t0;W-*H0I$"
"&4$gL-?sI3CU+T%6ea+7[8Vw>bm?[5m%xF#6.^310qFT%n<1'>VU6s0k2oO(`ZsZ3eu)Ab+Rlb#,eR?u5.Zm:.FOr_SgQbF9LCmuUTGPNuo52(3hmoNSI-6&i>h^uj4?=1V9G/(T$OPZ"
"Q6$a,S/P3PUbVE#8Oa>Ai8?G4maPfL7pg:dV4+gL18]]+4Y3>5hgQ?g@EK%#`b'C49[MOB^UX&#PMdb%?uA]$0o>JCZ-Bv-5xhp$$,KF*AbT)Zo`5/$MX1f):2Cv-R8YOq'><;?i%YZ,"
"#3Qv$]&3+rdptV-U-As$/9<16PB@)+nkx%$TGwASln_O1p^P11vACV%l%[1KO4.W$S8HIh1O5D0Zfu'a[SSYYq*u81lKM227H4/D6?;sHi=RK>GG^19WK$3+B.qP8[Tt#$BLis-%/5##"
"[_`8$0m+6#F8^kLB<LF*pxJ+*W[s:8A3*uJgfrG)x,Tv-unAT%fi^F*PC(*41t@p$n1%[7b.)N9nL=X$q@@w7&N8BHe1e%bC)d.GP)-Af,7;I,fO(fh__ZC#`4c40+Z:U57;=,MUGPU0"
"2d::5ND-hGq;&[pYPWJ1[V_'89QEW#,2RCum5n0#]`qr$XK'd2p[_Y#(2###&0wiL]0nY#[MoV-tvbM'Ox&vYj&Nv,2EXw0%.=G2AQ4,`om-<.F]tr-d6gw'`1`AP(j(B#STij22<n]O"
"D@TK1xhkYtvBhI3V3'Pol4N_&8RC_&K),##T6XE$HbDE-N56c%S)0J3^5W.35G65/_ZnG*[>:Z-4*11p_hb-6vDfFD'aCPSJHNw6Xp]n2PHex+`dw[F*E?+4<>@8@.Lif^vs/$?Ol),V"
"=+;;$K<5[$Bjxi9:J'1PMwPk+$xYQsjZ8vP*8###RH4J*:#G:.s))F>2p'?:ibeM_PdslLM3wu#rTclArqxY5d<ai0sg%##`4QT8T[eA,:WR/&B*^5/%[*G4)NUq8,*UR'Cp<c4stC.3"
"lYWI)x,Tv-&HnS/*mdA=?7h)3&hLw59BhQ&+edI)N.2C,>dh-2jYIB=_dC6&^cGD+sD$v.eHaR86Xhm&F1SW8T-(3Mu<Q).DFe8%ZO1f3JV)rAZA[g+22fEDdwE5&J_u31j<3S()rGQ8"
"APGM2jQx_6,(?<6T(]<$icX8.a6,wA?R[&[a@LA.dpFL:O?(s;OJ>i<,m>>#n#lRna'.m]A($##1XkD#[Dsu5oC,G40<+G4cFVC,M:CkL'dJ8%:X&m&sILP8a?:`-D*%[%n@xsLtgOE,"
"h5cJ:Q;mRn67Bn0Zm.k#01cW#i6RB4f^uSu<AsCN%/5##Z@N-$55+&#a*6?\?V&L/)k#6?-;'+C(iO0>?_YO]uIS4,'PC6##)E(v#KN0:#4f1$#C^qc)dW<?#P%1f)S^[:QL?WI_I(m-$"
"L$F;$BdGjuKmr=lXfButeeL]O)xH#?\?$H.62c4GMi6V-MZ2)5MEE9>G6+kfLbk1qu[3%;-01oH#1WD^ME:cS0:I:;$<5Z3#33*jLOreD*nm'EEkeYs.9i?X-6/NT%->UJ#4V/n&@Yfx$"
"KMDE3fNw>#-/1t#^mS22iQ*?&BE_vc.G@@bIEOJ:U,?p9r#vD:oQ2suxH:N=K;/m*&/.t-593jLF%W@,a_:W-&hi/uC`Z8/*-+c4qD_'=sLt87B.&p9%DGJ:+behD`vCoR5euD#7&J-,"
"tjT]$_V;-*XHl:?1njfYA[(A&^=Pd3/HiGuoTn/20O==%@oMxbD(/T.W-4&#bx=G-1eBDM;2_F*I=:Z$AL%LGU@pL(8Zf+4Q*.AOrhtA#ib8E#fDkgCA+'[urdhduu>EWH,+Im0aoaIq"
"(PL*B)FGtLRgCF&nvh^Y2>(>IMBJ*p%*IE3a0.pK]:*cdoHOCjE0.e-QrES3tlAr-1ECupN:?>#$B,W-l9iQ;98Uv-v>)6/VX'/Lnh<p9*m4m?RngV6:,S`$UBN%$JS+3+e+Bw2CRgD*"
"0fEhLC]ZY#j)fo.-dqcVO:/T.0V'f)kc3$%ef)T/i2Cv-#mqB#afLa4'PFb3oTw+'>_u]++-w--F3Nx5WsW/:X-+%7%mx)E<'vZEet`%A,LIUe,ZncV2ematL-96:g3%q7xCcx71]#=$"
"WYo#/KN<Y#KAAZ/CI/&#K1fQ-O:iT.LOlI)Z@r&Mx<i8.^U3W-0#)C&Bk,T%)xF`#GRYl3IKmCNQ+YZ#k[_Q&dq-P`dO9JLLp?_u`TULJ.E/b$=%nU&bSVZuvLY`u:-'8'[qXP_IZB>G"
"g>-jL$J0Yc^wPAOY:KS7`J5H3</4I)f>4B%%Qob4+e7:)Xc^gLOMr8.X^7eMh4Bj0e6fT%jLx:/SQ*c4[<An//xrx=BfR*3`hI_6]G_n/GsSv&>oxt%OKar3k-3Zu1O2#$D71H2bpMv/"
"RO8`uZX@E#uY(auX4RYGMA3`uD&N/GJ^wn5_WGFY@&7;8B.?ci::r^uW;9O0R*3EETM@@#4DP*EjMn21Ka^c2&iM=-Ma*vLld6lLCu;9/XR[E4b.u`4rV_Z-;bOU%%Nl>#25sK;K8;R3"
"u4(F;[-f,7Dj0;_T?om.C_$7'PsQF#h?jPg(/8_$=EZ8+V<*7;R(>'>:w%/+F(X^4Em^8//7Fx$-?vbd(_2^4o6I*RoRap.1/4&#Qcqb?gi+G4E0Hs%hq7W.OeUN(HgrB4Z0k8&`Hfj'"
"]08:fk-gF`>Ou/(Ngtj01(r;1&P^C+Y`S4(s6snOp=/h_L:EN3A(/YcEJxY,Oxqo%JC$##Nl^c$'K8f3-.#rCfj?#6qEIi&L0,D#o4#]6<o>(jL28b3&Oilgx$wo'^;<1B#<7r2_Qxx&"
"YSS90m`o`t-M($$e%o<$?e0ct1D#j*4lUA$M/(UD2Bt$M1O1Y$lb&0MPYDa-+o<E,Pjsn8m*#d3a+r;-]<Nl%%8WFtAF%Ej5ukE4WCemSQ<E%$oV2N7&S]S65Q4Q/SR8`uo5CPJMA3`u"
"afY$%p4]l&E5=fq-`[>uH0BK:#w25(9;qCauuId;1ZI%#jTD1)djB:%<7D8.@Dn;%Q7h8.swK_4]I&@o'?qZ7mrD:2N*Xw092iR/TN4_?G&J.h<-L+MLF,B$J28juAce'1jK&W$PEQt:"
"hs$8#wnIh$uEpkLI>^I*OP,G4aN=R%T&sS/+aL9.GMrB#eB-U%Q^2hH,8+)>3uU+-A/_30J?=^PC@&n/N/=^OH=4qI]3PK1_1]R=fGT;1MLZL6fEp17vJo#I:HD1(Xd+v7Os/i31pi(4"
"3o05(*KL5MqPj^otto^onLdERhl###s(]L(l,+c4J_0$fB8xiL1S%:/G5R_u[&aZ8D(K&$9p0I7]+F*&G;Mk#P59Z52/3u%U5CA=gdPx$;>I&4#4n0#e9]uGI9U2U[[-5/8teG3tE^&F"
"USO*?F$2x5'x4_$0[]p7Ql#?##1E@ufjJ=PQBG#>/kvX-`Ww>&l>kt.Ke$X$1CJ.3]1F1?ouf`>'d@^;mXK:uhIU$$fuRw#0008'RofBJO)N^&Jq)wCf8QK)Af)T/$A_s7p,R@'@hF`#"
"#9=CNpcH40kSN48(IS`4lBW?#3+xiTRE=gt;Bqv5_%U#0_La_tf,hv5'?2];a3QL/NI:;$xlovL`p0;I#ZZp0u[oL(cU4HtgxdC#FvCBoU>>+JLlak61BBY#EFDQ].V[MTPrY`u;%x$8"
"p3PU&9aT/1D2fCNs`-@d@Ft39k)^G3aMa-2$Fo8%d(4I)R4sX.N,'J3A;$T.#3m;%FD#W-<EpjMgY'3CBKYx0LIk^4HegnpsSw$;.l?=-K6Y`uu:&s&<@n8%u%Eo3nw,jL#9n#6sCLs&"
"O8?+5lv4^&lB,j>TqdENkJ8*3VPqt&KW;/6Y:)'m;rUNt*X=<-+O<a%EXl[-N=+D#H1.'3hc6_ueMY`uM&BK1@C.^6uW:R'I<grmO.etTYfp$&;&u;-*^(&M)B]s$@0Ct8dTml.MD^+4"
"OnYhLg4SH#t1Op-os<l?l,c5&c0T+?]Mf_5`v$^X%v:i@3&Y,fDDxiT5V5(j7w#vGe.cV-)o/m&2TQ+Sm#TG7Qixp&PVcl(ao.@#?,Q8;+1qW/&5>##LRUV$D0Q3#fd0'#Qn0$e*SrB#"
"Xnr?#mDoEN6H:oKA.w<($VHp.0H>wToVmD#sf6<.NO6##+G[=?eckf/--]in:k056q]L,*FeX25k.P4g(F=^O`cviT.'@&SB,@euNdPt$CXn=lX#Rtnn3r[uX]T4JCSXcM9i,QukU]>-"
"4*$v%SjZd&AUt2iAtHg)i6a2MS(_1&[CMd32`NT/9ik,2H^;F3gF6w-SZfj'&H%rS;1`A6Wu^lgNsl]6ZjUe*8eP?(T;+?I9GOJ,-oPX3<f4K1sW;)$19Ol8B3CH2rfxE.x1jZurTUQ&"
"2rg0IHDo)=P2BfhW),##*C8A$*mx2DTshx$Uc-F%E?`F*xnh8.+_#V/m_Nx-r,+c4kRN`#P5SA4nN7S't$KCuP?o>#hb>$(`$t=$Nudj'kv-l'XJxE7I<Cw#.(wP/UVY4/6;F&#SIW-?"
"_Q/o8^Cu`46S];.8:FT%&/OCl.]l=ZjAm]6w$LJL'iuX1Qd+R/IXv>0&F<;.+U5:&.&t*iB3cK1=-Z[R,>bY5hpCl-hGbKN(J1NCdp?#6F+D*%rgM,M2AC:%%GNT%KE2B4Vg/Y-blF20"
")4LR4V4[Y[dDUl/riXr'['Tdt$;Hw$GP3u%C5wxG&,S;6]B1bt-%Ea-EBb@$pUKfLNR-##)+/x$?/Q3#9%(&8mrA,3&U/T.s'#V/_J5d$M<q-2Z(8N1qSlD#H)H<6p#Er&a+B-)n:Bqu"
">S*-M/+S0@J7+8nQPqKGS)>GiTWqd2h#?2;N#eI>u=/j.#/$.4B1Juc@9f,3ndw+;)8BZ-IO@X-M^dj0Ygx9.-moF4%d#W.86LT%wx?8%:RFG25TW*.20>.Ob++r^$MM?#fFbA#cRkfq"
"^Xk`W:7Y_bsuL4n0.mB$,?)t6PmL+M9SE1(8%]%$GXb7evSN`slkUs%d-U9r>77<--ReE.u29Z-&N^D49&/*3^E&^6ATQw'+):8.1ax^/@UiFMdTk'GrR_U>]7qw,S;HH2*s%n';p5g1"
"@x/au>/R(G1JoC?l[$^5Y1uUQ(gGpo;p?##2@WYdh+&^,_)c/)$THQ:w&C99UNv)4@6=a%Mm7_4'Wcc2<#e;>ZUb]<gpD>,gFS^XJIxiTY[);$MP^[uIb9b-:5mG2PUPFu3k%J2lcWi)"
"$),##ROo'.E=J[:KiJ,3Qr1Z=Ub1e3:Qob4lTAX-`$UfLBxEb3^-c>H0Q9T7NO>Urdxx)Clij07.&lxFa;A>uL-ra#@xlqg0]05(Jrbg6pM5eQE<bl(KU-h,Pc5?&B]Jt%W^mY/L1Un'"
"=1=(MxhBm&ti7Z-^.X6Nu59H3+J,G4aYfQ'Lp;tg,%oC#U97r:+/D9/x?]s$xn-sAw6pRr+MsF;YK7mQJA6U.pINV?$(?<eWUO]uFLf-,.DXg1Q1]P#YUf4)H?,99mbf8=fUM<A/2<xX"
">#3[$pS8(s]A9v0ipg7&2>@x':,(58nBH?$@)H3#AWajLE6iM:'x:9/WQBv-4%hDN5j?lL<a2#2T^bX$%Bh>$<HWl]ri>4MAdj`upBBWnsLA=%RrxMc?D2##oD9xkh6u#%O)?d2/;/V&"
"A-89$0V_7R>BUB#kYa:BP6h=Q%HH+`+r2_#OIeT*t#(<ew`&AtK*NP/p.7N9%lW>-tQSs-T-xe*ftn5/]AqB#'7*iMjqf(%womD#w4NT/Ssqr?p-rE.iiEiu^wM%ubq]>Qt3%CQd&'b*"
"$Gfp&VdV^`D2YXZot[h&-Sc)$IUo@?'sWK66YAn0eI=q'^SYx/D;wH+T3$R=+l^j]>;Ev>2-PX;WDWPA2*a1&*bWQLpYC:%t?]s$^,T%6YA%^$qKo;-=#d,%[G;e;b52N9srL&%`x[?#"
"$hA]urB(duPFY?#5>ZYblMfXdWTk`8^^'eAa%:*8E5NT)pMh_ZTn$JCSHS7&;7M#$Lovq'E-i19=IHgL$D)G`2ClUIi3n0#vXb7e?nDAF-j))*kE*20(;HW-ga44M=JtA%:2Cv-`aoI*"
"*2pb4O/`#$.%I=7`^#V/rbD;$HIt#%&&vF+$u[bugW[]SR.16CS>eg6'./X.`j7E.:Yot/wjK60ebgN)mI@g:0N5=@%50quLuD,NH:SG;SUT?dxlSxt6s-,)WKBD3&LxX$P3+L:r<CW-"
"#>'v5`vauROYl`#/.^s5TdSQ/_),-;T/%:&@$)VVxgR$9t_'SeGk5s6HeQS.wkBr0FMG`%EEmZ-hEB:%&X75/lg1T%K-+c498tM(DM[v$agjL#;BNs-T@0&$MecRex(kI)(`doBJ1[>4"
"?-mS+d4:c/rk%NT+=?xOtEbs'jZ9^@><NY&sn0n'j&wY#eu^u%XV+)Etk.K:qxc8.U8))W;Tt;-)@7I-kZXY$eo@A4xk=X(rhX;.vX#gLT1fX-D$B=&#I,b#8xwiJv2:BurN5x,([R[6"
"03f<=h%e#0,E$p#PY?>7LSt%%FP/n4?#*EPmU4p%efTVSXn1p3LK9R`#[7(seKK03+H<E[qkx8#pkErL^GF)#<ac20,)9f3=)]L(lC=5*%(:+*CpON)A`?<.R4D'S4aZv$]3B:%UekD#"
"Zb=Ug2W@`$k`SN8.&F:&:AAx$rj=6/AmGg12E.F4+Oqe3*r:D5U5LK1./9AuLQ_^u'dHWg5o[pOSm`bPn1scbJ4G4oX^YP&WdCh.Bal)kGPo7CEfK'd$[I5/U/_c2x6dq7TWHdFp]Z.%"
"l)9o88e:4DbYob46DYjLN0h-4AA5c_K`eV82GDR<2J551C%i)/g:S[Hl$lp%U8A*7.*bY%fWHg,oa%K3n=ZB,M6xZ.+$ST.K4io(_Y>K*AgH.<]gML1o?RpKmRS8#QeCxL3eu1:?4cKN"
"Wv^>$KKFA,-V_R(_5,l?=n2J,[D:01YdRx/'3qI2Z2%:&j>@BPm)k.VM5mrHQa`&.qrg%MUtYM$Ma($#)o-K=sal8/3'b/:&_:E4SdDb3]JD-O5]Mu$m/D]XsT]V8gs%U'MIoW*IbV65"
"Hb,W6qK]5hDp3@$+6jw$EK2^4[5$R/I'7=0]vBV()uf'$FDP4SolfQSEXC;6o'CwKaiMxb$<'d3wLpP0ql1g:_gUa4+g^I*V/NqM%>:8.m%]u8;5>)%GInes=$bYlXn930eC+q&vZni("
"D+R+%jSWU&a7rv#*lM4fOcbYHAO=,2sX>e5l^ud1l*s20ns/5)mmJ43OkcS7=hdp-CJ2_J-R0_JEP`$'K=5i(Z+t;.hg+)<0xPm8U>Zv$U@u)4-?=c4[=MXeRlFV&*<]32PWQ=?Wa[S'"
"dmTu$XjID4WhQFoEJR</w)xCjPHxq9pN2_(qEMf#,Mv69WBZ1/3#%)W<S]wB4)qQ8bCbw7HYY`%RUsEINOFb3bs.Pfe,U]I03&7&:GJx$gb5n&<WN>5<axG*gE9$$@c$GXYYTbA0lT70"
"9h/89r`;U)SjIi(Y<lt%PmFD<@?6$G4rs[XRdH_#:/7S(Qp+<-1=Nq-PAjeFDN?,@3xsY-oxd20;2WD#XsG122+N*I]nQYu#5O>5[GLK1tOjo@ksrkfKG7>>CCkp&E=gV6K5YY#>w)##"
",a38[YILS.RF%N9?6ts8?cuR(JX1x5sO,G4x9Q127@[s$q-7V[=Isj?M-sJ,>N<F]0fi,``H^h7$BWv#&$/w7NP;I?Degr/f>H.?+X;suLb6w$oJ.i(C3<'/]Tf='cF&?@NW2;ZZS`A,"
"g.X;.70039YfOg1[5T/JZ5:7/(.k=Rlq;l(xW;$<V=,k5in/L7'HADE8'74*PZMj5/D6;HUKthL=UJouJLuu#?(mb<WN@k;,Z;E-f5-I%oG3I)x1n.%B@vr-oq@T7W9-`M:eX?A(#ON0"
"An/HB_qF?,)>,^k=x:&%Y=gj5/QMFP9C-buA.UN0iDHi>):drua#&#H+lo+M@TrSIlbKk.E@uu#EDeH&otD)4t1RQ/K2'J3sJ))3#bqT%f';(vGA0&$aW]in8&kv2veHu&Z0+x#`&sdC"
"?XS<1eZ18[^%;o0aw3?'aJMZuC$Aq&/+eW6fJI[PGdSo[P2qiLvS-##K@uu#g@u6#6v>t$xAqB#DP7T%n^DE4#@.,;u%#poGBQu%LG:6&jDk9.[]ACsQb[>#^,4F7-EV7e`nV9`%6k;-"
"%JkZ%#kn+>rjP@.s^v)4D43g)W):W-g4LgaaOQI4bq,O(W3&o^r<1-2P7r'<J':OJ.M]t%mU)c>M(VFA:@01m<c8w#9;`@OrdfdDhK<AFJLH+D1(3IVi,7M;cu6gDlA^P<5+`a#-GM-Q"
"ob'SeJrxrZ(ICW.7c3>5?O`=?(iV6jHCTfLZWkD#_S;N0t<7f3eUKF*pP>D<H7N;7%mc/CDU[n51PKM*14ctZ<2Zk0t1nfRA1dk.GxD%beKu%Pe@Zw$Eq$PO,q8.$motm^7Eoo^'pwuu"
"lEHZIfN)p3pcZh7cUn^u@RGe,_FndBtO2J1*HdJAH),##?@uu#Pr=6#(==g$OcaF3;MH[&)Nc;HvdCC&chfw'F+k:6AwGH7O/dA@-4Z$2xPi`5;V)2^GW_LpqEHF#/gQN'j[th88wUau"
"CJL6&F5S_4rjkUm<W(C&F_A?7r3wr-L7+_JN4kx4V%3_J(N6T%foEb3A)]L(,CV@,bhUs-*,`Z-kUFb3B3ul&Nr_m5[#Xx5f4E:miA(2^X1:=u3;BkLSwHIqa`5:A,[H3B(fG'$Ai*v>"
"')h#$E?u6#U$s,3GF1x5YR%'M7;Bj0IoXjL4:oC#Bo.E3H$L43>JVS%NESv&DQWv#Vav_jn8dc2>n=01dmh6'<9.AO0^L$'E:XV/<P$<-I]AN-OhDl%vuY=7Ol3[8b+l/E:/:Z-Jc.T%"
"GH%v53odn']C[lF5N`>5DG5p4YdkXrnW&2^Ps0]=uB)JqO66&4_nO^$t9Wc8;[=SLnN79*#5O_1wX15/C8e4v48ofL7ou&#vurEei^iI3V*,30JIV@,p;Tv-1ISLGU%IK1Z=-aRw&hf1"
"k$&t-^#;-23pP4S@Ptv#glwI:i=8t>CELLpoqxX%DZX&<a92v5OtAK-))9W3%/5##F@uu#`N]5#Svkp88e5g)0*rx$mnJT%QEOZ6n%NG)e,]F4rFxU/Ti(?#NNeh)`o_IcMar':J87n)"
"o?YP&Dk4]Ya4C]Y2(I/7lZ@X1j:kjH'B(%0bL;@&2H_Bu$pZx#XoLJ17K)Z>jR#(#E5I8#xuF:$7[u##p-nguXW5*nUv+W-9;rdmC;HE4?caF3qEQcMRru'$<H%&62gh*3j0utgCdmm6"
"4-Ov&acYl,;x6>Y^;-#$c7('uv[-n&pgV6196TGjr0R)+Uc*.6Q-@d*`b*CuN_xF<Hp0m9j9<YJuLJ88wxAgLfNGf*EWSb*HBb/:<cp58V:wo%'B^_%EGKqR?lw'CZ>GlVcId4,Q6U?-"
"6K(2^aPFF1DO>]u]eDB-qTjc#21tZu;f9.9D?g&=NknH+GOWoIx`=Ppo^*g2k,u9.-s^I*s1])3OwAv-,7F]-/Y<9/hj-H)/EN/M$[8K4Yh=HaPT;ZD-0fm*RSq2^In_&;)jcp2C_x1K"
"/+xMp:Q_q%<m=&<LcO$9)u%7L2>(s3v*DT7,58TIg^me6Xl-lF2D83KQ8fV6,UL%#Iq^4f>bm`EL_UV-kE7eQ0)]L(LD,c4K%@t-->4a=_8cD4?mAq$mJ+P(J0@Z>Sfu>#WRwR9ROe?."
"7*39]<b&8HNk%&6,RjJiG6h%$%(#XZ&x-8[@;:lSjiFJ+vfS_usmR>/mBANUIb':&L$%m8BX;suFUhv$s1j?#?ULv$X@4D<WnK#$Np#-.:BN/Mp)VRBxN0N(Jh1dMK[qh$d*c%$Z=61M"
":QPG2p$[g13Vk,2FF>`5J0pUp]dHWjeKW$$7(WOfN=clA&e8,1hPhB#St%E5hF'S3(KuE9V+8H<EC:(#IGe8#sY+U$]a($#nid(#:-:53#oh8.iu@A4.DXI)pT)*46E^,;p%tY-+HlG;"
"-?)KC@_8H2NAT`57U1e3v>D-2gOO3JAi3iKFOoP'O(O22pEBoCRbu2q#&OA4R[v;/0En=$2r@m8X?vw5>9F'6B]FQfFd]W]j=%5F,)s2KCs%n=dAf@?OPI<<`r]WPI<lL)0FpQad.Juc"
"3_ORpgc_c2%+u`474gm0Fp?d)$g(T/;vWZ$-;7<.<pnh%flF+3nVe)*K29f3elm8/:%`v#&i^F*^5<<<Ea0H2;%4310AZlhE62w%(4s)%J8PB5jVOK1J@iJugMJQ#K`QnaTB/oURZvE3"
"2;&N0's%p%rT2DNrH7K#KYVx7kSae3U;-_>'EMb5nGWfL<W6##CRu>#qUf5#A%@p7nxoG34@xZTMJ'i)d>Zv$^p*P(m[&6&MRhr&KdDB4GvD[$w>9w#vwCw68Ri%6'hrkf5c)6/rNMkL"
"BG?##gi>%.U&OZMdac&#eh74=mh%s$6$qC-&wpO0V'Y:&R(,>%ACD.NC`S)&Sk?8'k'Xn/=BD61BAda5sS8F->%)IMHK:2#x.:7%HUd.5mhXb3@=Vs-gspD3cag:/;op>,w'XD#eDIB4"
"V@vZ#D@]/1oxjo7X9$E3KfVd2%9sFNXME_$*Qp<$0<e<?3Bjc4V2=<6sR[F%AuF'#1tO6#aIJ]$00PG-?:Gc-.BC.?7SaAntl?##DI:;$3kN1#BL7%#ZCsM-IjHi$.moF4aBdl$GJ)C&"
"uBIaH$$3E4_JRv5MNH>%f48x$$,.F%X&ME#jLrp7ijw-Ab>[Y2hVhBuHmo.3IQ.9'7'Biu@6&A#k7'aNc)N&5m0[<&X_1n'O$FsomxVWHo:S+Masq)&T(J$$VLK2*vIPS.O#.8#2xAgL"
"RU$lL0=`T.,Sl##o3xU.);G##j.)t-t8/cNaLH##=F`T.XD-(#;Xx>-8X`=-^xWN0mIa)#]6>##_RFgL_S,.#NB`T.57%@#a@`T..]1?#q.)t-8+reNaLH##JF`T.4pRH#SXx>-8X`=-"
".EfT%Sa2MK(`YxIl*m<Bae@AYTc>jF9i%mB'*s=Bc)+k1,Beh2=g0s7ogVD=&>.FHoZvRC:1k(IA)NQC*a1eGrRN88H(5e5pkn+H5-xF-8-IL2]w#@-lhwFHiN`TCr6/+4Q5TO2G6<LF"
"wj2=B2rCVCuY(*H$A'C-Y1wB6h=r*H2.vLF;lY>B77sdGoAXVC;W1,H-U7F-xRV=-'Op01^8>&5^fuiC='oF-U82E/tJa7DKREkLZOhx=c0exFDDSfC=sViBH@9?-BC:;-8kTq),@AVH"
"#EG_&p:H]F6e)F.k+-AF[N`50<kt9)Vm>G24Ar%4l_As7i-7L#mj0A=IYo+DUw%;H:eHe-d'j9;<Un-$.R]rH14O2CD.Me$uV<MBKve]G/?m>-?dY-?pGs.C_1IY>P5+#Hu:k`FI2[>-"
"M=-j1UbdJ2kXk>-*?Pq2J<EqDpx_?T`kq34QQ'/1-tQ_&)<,R<V'A_/c@D3kMoOk+p(Jk45@n-$vqoQWx@;GR7hEFR7:B-dN0U-d'=P3XvwY'82du7I'l,#Hn5-mBbqWB-QrUH-9)m<-"
"ceF?-A<8F-8gG<-=?(@-2_x>-B)MT.&vv(#sx33.,#@qLBe=rLoLG&#5.cw$jZ8G;a#;/D67xlBv3)KEK=6##vO'#vfV+WM3`^;-LZ:]$MDuu#?^Z`*IkqH/KA*1#>1X$MD9(en8h?YP"
"LS-JUrBB`W3#T(j:6s7R*J(v#H#5PSpL7]b]AK;$mW:YYdQqFrhod(WEH1VdDMSrZ>vViBn_t.CTp;JCbMMrdku.Sek+f4ft(XfCsOFlfOuo7[&+T.q6j<fh#+$JhxUwOoErf%OLoOcD"
"Q@h%FSL-AF3HJ]FZndxF_6auGcH&;Hggx7I1$BSIm/YoIrVq1KXpa._D1SiKx%n.L<U=lox/Ff_)OvFiaSgV$1*8`j7TOxk2?4]k4;0P]<lGi^G%Ll]I<p=ci1r@bVqTuYLwwCa-lkCs"
"2%@8%6=wo%H)PlSg:2MTWd:ip[>XrQh6>]XM&ToR:UWP&>n82'B0pi'FHPJ(Ja1,)N#ic)R;ID*VS*&+Zla]+_.B>,cF#v,g_YV-kw:8.o9ro.sQRP/wj320%-ki0)EKJ1-^,,21vcc2"
"58DD39P%&4=i[]4A+=>5ECtu5I[TV6Mt587Q6mo7UNMP8Yg.29^)fi9bAFJ:fY',;jr^c;n4?D<rLv%=veV]=x':oDM]#03Ys]G3`SUA5j1D.3fdo`EgQ='%'HOVCB'K29Sg[7Mmvvh2"
"kr6F%BvIL2h@5dM@*3i21/R]-$OHR*0T3i2JS9I?4jje3tx>)4'.bw9$Mv`4gc:&5b7$=LA4wt7qhJ88jXfS8kb+p8lkF59mtbP9XpJ,3VY#<-iIP)N&p).N&p).N&p).N&p).N&p).N"
"&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N'odL2uvjG3iIP)N'v2.N'v2.N'v2.N'v2.N'v2.N'v2.N'v2.N'v2.N'v2.N'v2.N'v2.N'v2.N'v2.N(umL2v)0d3iIP)N(&<.N"
"(&<.N(&<.N(&<.N(&<.N(&<.N(&<.N(&<.N(&<.N(&<.N(&<.N(&<.N(&<.N*.<i2v,B)4gQA)4_9A)4YbdJ2O2YR*w=d_/Q18(#$uTB#%;cY#$,>>#,nnP'2NgJ)IQGSf&l'B#w5m+M"
"jW&%#e-LhL@nWN0NV%JL:tL%b(d(GM3'tN0IeH,v0'v5vAg(*MYEfo%h;#MT[2WN]J^k'&.]^:#]SMwXdUs)#.K)uLdCZ+`VoBuu=[C*v(SUV$6U0>]H$###";


