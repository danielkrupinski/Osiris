#include "valve_parser.h"

namespace valve_parser
{
	Object::Object(Document* doc) : Node(doc) {}

	Object* Object::ToObject()
	{
		return this;
	}

	std::shared_ptr<Node> Object::GetKeyByName(char* name)
	{
		for (auto& child : children)
		{
			if (child->ToKeyValue())
			{
				if (Util::StrEqu(child->ToKeyValue()->Key, name))
					return child;
			}
		}

		return 0;
	}

	bool Object::Parse()
	{
		std::shared_ptr<Node> n;

		while (*_doc->p)
		{
			//check for object close
			auto string_begin = Str::ParseTextExpectedTag(_doc->p, STRING, true);
			if (!string_begin)
			{
				auto obj_close = Str::ParseTextExpectedTag(_doc->p, OBJECT_CLOSE, true);
				if (obj_close)
				{
					_doc->p = obj_close + 1;
					return true;
				}
				else
					return false;
			}

			if (!_doc->identify(n))
				return false;

			if (n->ToKeyValue())
			{
				this->children.push_back(n);
			}

			if (n->ToObject())
			{
				this->children.push_back(n);
				Object* obj = n->ToObject();
				if (!obj->Parse())
					return false;
			}
		}

		return false;
	}

	bool Node::Parse()
	{
		std::shared_ptr<Node> n;

		while (*_doc->p)
		{
			if (!_doc->identify(n))
			{
				if (!Str::EndReached(_doc->p, OBJECT_OPEN) &&
					!Str::EndReached(_doc->p, OBJECT_CLOSE) &&
					!Str::EndReached(_doc->p, STRING))
					return true;
				else
					return false;
			}

			if (n->ToKeyValue())
			{
				this->children.push_back(n);
			}

			if (n->ToObject())
			{
				this->children.push_back(n);
				Object* obj = n->ToObject();
				if (!obj->Parse())
					return false;
			}
		}

		return false;
	}
}