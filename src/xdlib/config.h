#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
//#include <sstream>  //���Ϊɶ����Ҫ�ӣ�
#include "constdefine.h"
#include "point.h"
#include <QString>

namespace xd {

typedef std::string t_config_option_key;   //����ѡ��������ַ���
typedef std::vector<std::string> t_config_option_keys;   //һ������ѡ��������ַ���������

class ConfigOption {    //������ѡ��ӿ�
    public:
    virtual ~ConfigOption() {};    //��������
    virtual std::string serialize() const = 0;     //���л�
    virtual bool deserialize(std::string str) = 0; //�����л�
    virtual int getInt() const { return 0; };    //�õ�����
    virtual void setInt(int val) {};            //��������
};

class ConfigOptionVectorBase : public ConfigOption {    //����ѡ����������
    public:
    virtual ~ConfigOptionVectorBase() {};
    virtual std::vector<std::string> vserialize() const = 0;
};

template <class T>
class ConfigOptionVector : public ConfigOptionVectorBase    //����ѡ������
{
    public:
    virtual ~ConfigOptionVector() {};
    std::vector<T> values;    //value����洢��һ��T���͵�����

    T get_at(size_t i) const {     //�õ�values����ĵ�i�����ݣ���������±��򷵻ص�һ�����ݣ�
        try {
            return this->values.at(i);
        } catch (const std::out_of_range& oor) {   //out_of_range֪��at��Ч���Ǳ�[]���ʲ���
            return this->values.front();
        }
    };
};

class ConfigOptionFloat : public ConfigOption    //����ѡ�����
{
    public:
    double value;  // ʹ��˫���ȴ��渡����Ϊ�˺�����perl�������ּ���
    ConfigOptionFloat() : value(0) {};

    operator float() const { return this->value; };
    operator double() const { return this->value; };

    std::string serialize() const {
        std::ostringstream ss;
        ss << this->value;
        return ss.str();
    };

    bool deserialize(std::string str) {   //��������ַ������ݸ������double���͵�value
//        std::istringstream iss(str);
//        return iss >> this->value;
          QString iss=QString::fromStdString(str);
          bool ok=false;
          this->value = iss.toDouble(&ok);
          return ok;
    };
};

class ConfigOptionFloats : public ConfigOptionVector<double>   //����ѡ������ǣ�������value������һ��double���͵�����
{
    public:

    std::string serialize() const {   //���л����value�������double���͵�����,ͬʱ�м��ö��Ÿ�����һ�����ַ���
        std::ostringstream ss;
        for (std::vector<double>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            if (it - this->values.begin() != 0) ss << ",";
            ss << *it;
        }
        return ss.str();
    };

    std::vector<std::string> vserialize() const {   //������value�����������ȡ�ŵ��ַ����������沢����
        std::vector<std::string> vv;
        for (std::vector<double>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            std::ostringstream ss;
            ss << *it;
            vv.push_back(ss.str());
        }
        return vv;
    };

    bool deserialize(std::string str) {   //���ַ�����������ȡ�ŵ������value����
        this->values.clear();
        std::istringstream is(str);
        std::string item_str;
        while (std::getline(is, item_str, ',')) {
            std::istringstream iss(item_str);
            double value;
            iss >> value;
            this->values.push_back(value);
        }
        return true;
    };
};

class ConfigOptionInt : public ConfigOption    //����ѡ������
{
    public:
    int value;
    ConfigOptionInt() : value(0) {};

    operator int() const { return this->value; };   //���ر���value
    int getInt() const { return this->value; };     //���ر���value
    void setInt(int val) { this->value = val; };    //���ñ���value

    std::string serialize() const {
        std::ostringstream ss;
        ss << this->value;
        return ss.str();
    };

    bool deserialize(std::string str) {
//        std::istringstream iss(str);
//        return iss >> this->value;
          QString iss=QString::fromStdString(str);
          bool ok=false;
          this->value = iss.toInt(&ok);
          return ok;
    };
};

class ConfigOptionInts : public ConfigOptionVector<int>  //����ѡ�������ǣ�������value������һ��int���͵�����
{
    public:

    std::string serialize() const {    //���л����value�������int���͵�����,ͬʱ�м��ö��Ÿ�����һ�����ַ���
        std::ostringstream ss;
        for (std::vector<int>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            if (it - this->values.begin() != 0) ss << ",";
            ss << *it;
        }
        return ss.str();
    };

    std::vector<std::string> vserialize() const {   //������value�����������ȡ�ŵ��ַ����������沢����
        std::vector<std::string> vv;
        for (std::vector<int>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            std::ostringstream ss;
            ss << *it;
            vv.push_back(ss.str());
        }
        return vv;
    };

    bool deserialize(std::string str) {     //��������ַ���str��������ȡ�ŵ������value����
        this->values.clear();
        std::istringstream is(str);
        std::string item_str;
        while (std::getline(is, item_str, ',')) {
            std::istringstream iss(item_str);
            int value;
            iss >> value;
            this->values.push_back(value);
        }
        return true;
    };
};

class ConfigOptionString : public ConfigOption    //����ѡ���ַ���
{
    public:
    std::string value;
    ConfigOptionString() : value("") {};

    operator std::string() const { return this->value; };

    std::string serialize() const {
        std::string str = this->value;

        // s/\R/\\n/g
        size_t pos = 0;
        while ((pos = str.find("\n", pos)) != std::string::npos || (pos = str.find("\r", pos)) != std::string::npos) {
            str.replace(pos, 1, "\\n");
            pos += 2; // length of "\\n"
        }

        return str;
    };

    bool deserialize(std::string str) {
        // s/\\n/\n/g
        size_t pos = 0;
        while ((pos = str.find("\\n", pos)) != std::string::npos) {
            str.replace(pos, 2, "\n");
            pos += 1; // length of "\n"
        }

        this->value = str;
        return true;
    };
};

// semicolon-separated strings
class ConfigOptionStrings : public ConfigOptionVector<std::string>   //����ѡ���ַ���������
{
    public:

    std::string serialize() const {
        std::ostringstream ss;
        for (std::vector<std::string>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            if (it - this->values.begin() != 0) ss << ";";
            ss << *it;
        }
        return ss.str();
    };

    std::vector<std::string> vserialize() const {
        return this->values;
    };

    bool deserialize(std::string str) {
        this->values.clear();
        std::istringstream is(str);
        std::string item_str;
        while (std::getline(is, item_str, ';')) {
            this->values.push_back(item_str);
        }
        return true;
    };
};

class ConfigOptionPercent : public ConfigOption     //����ѡ��ٷ���
{
    public:
    double value;
    ConfigOptionPercent() : value(0) {};

    double get_abs_value(double ratio_over) const {
        return ratio_over * this->value / 100;
    };

    std::string serialize() const {
        std::ostringstream ss;
        ss << this->value;
        std::string s(ss.str());
        s += "%";
        return s;
    };

    bool deserialize(std::string str) {
        // don't try to parse the trailing % since it's optional
//        std::istringstream iss(str);
//        return iss >> this->value;
          QString iss=QString::fromStdString(str);
          bool ok=false;
          this->value = iss.toDouble(&ok);
          return ok;
    };
};

class ConfigOptionFloatOrPercent : public ConfigOption   //����ѡ��������߰ٷ���
{
    public:
    double value;
    bool percent;
    ConfigOptionFloatOrPercent() : value(0), percent(false) {};

    double get_abs_value(double ratio_over) const {
        if (this->percent) {
            return ratio_over * this->value / 100;
        } else {
            return this->value;
        }
    };

    std::string serialize() const {
        std::ostringstream ss;
        ss << this->value;
        std::string s(ss.str());
        if (this->percent) s += "%";
        return s;
    };

    bool deserialize(std::string str) {
        this->percent = str.find_first_of("%") != std::string::npos;
//       std::istringstream iss(str);
//       return iss >> this->value;
         QString iss=QString::fromStdString(str);
         bool ok=false;
         this->value = iss.toDouble(&ok);
         return ok;
    };
};

class ConfigOptionPoint : public ConfigOption   //����ѡ���
{
    public:
    Pointf point;
    ConfigOptionPoint() : point(Pointf(0,0)) {};

    operator Pointf() const { return this->point; };

    std::string serialize() const {
        std::ostringstream ss;
        ss << this->point.x;
        ss << ",";
        ss << this->point.y;
        return ss.str();
    };

    bool deserialize(std::string str) {
        std::istringstream iss(str);
        iss >> this->point.x;
        iss.ignore(std::numeric_limits<std::streamsize>::max(), ',');
        iss.ignore(std::numeric_limits<std::streamsize>::max(), 'x');
        iss >> this->point.y;
        return true;
    };
};

class ConfigOptionPoints : public ConfigOptionVector<Pointf>    //����ѡ�����ɵ�������
{
    public:

    std::string serialize() const {
        std::ostringstream ss;
        for (Pointfs::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            if (it - this->values.begin() != 0) ss << ",";
            ss << it->x;
            ss << "x";
            ss << it->y;
        }
        return ss.str();
    };

    std::vector<std::string> vserialize() const {
        std::vector<std::string> vv;
        for (Pointfs::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            std::ostringstream ss;
            ss << *it;
            vv.push_back(ss.str());
        }
        return vv;
    };

    bool deserialize(std::string str) {
        this->values.clear();
        std::istringstream is(str);
        std::string point_str;
        while (std::getline(is, point_str, ',')) {
            Pointf point;
            std::istringstream iss(point_str);
            std::string coord_str;
            if (std::getline(iss, coord_str, 'x')) {
                std::istringstream(coord_str) >> point.x;
                if (std::getline(iss, coord_str, 'x')) {
                    std::istringstream(coord_str) >> point.y;
                }
            }
            this->values.push_back(point);
        }
        return true;
    };
};

class ConfigOptionBool : public ConfigOption    //����ѡ���
{
    public:
    bool value;
    ConfigOptionBool() : value(false) {};

    operator bool() const { return this->value; };

    std::string serialize() const {
        return std::string(this->value ? "1" : "0");
    };

    bool deserialize(std::string str) {
        this->value = (str.compare("1") == 0);
        return true;
    };
};

class ConfigOptionBools : public ConfigOptionVector<bool>   //����ѡ�����ɵ�������
{
    public:

    std::string serialize() const {
        std::ostringstream ss;
        for (std::vector<bool>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            if (it - this->values.begin() != 0) ss << ",";
            ss << (*it ? "1" : "0");
        }
        return ss.str();
    };

    std::vector<std::string> vserialize() const {
        std::vector<std::string> vv;
        for (std::vector<bool>::const_iterator it = this->values.begin(); it != this->values.end(); ++it) {
            std::ostringstream ss;
            ss << (*it ? "1" : "0");
            vv.push_back(ss.str());
        }
        return vv;
    };

    bool deserialize(std::string str) {
        this->values.clear();
        std::istringstream is(str);
        std::string item_str;
        while (std::getline(is, item_str, ',')) {
            this->values.push_back(item_str.compare("1") == 0);
        }
        return true;
    };
};

typedef std::map<std::string,int> t_config_enum_values;   //����ö��ֵ�ǣ�����map����string��int

template <class T>
class ConfigOptionEnum : public ConfigOption   //����ѡ��ö��
{
    public:
    T value;

    operator T() const { return this->value; };

    std::string serialize() const {
        t_config_enum_values enum_keys_map = ConfigOptionEnum<T>::get_enum_values();
        for (t_config_enum_values::iterator it = enum_keys_map.begin(); it != enum_keys_map.end(); ++it) {
            if (it->second == static_cast<int>(this->value)) return it->first;
        }
        return "";
    };

    bool deserialize(std::string str) {
        t_config_enum_values enum_keys_map = ConfigOptionEnum<T>::get_enum_values();
        if (enum_keys_map.count(str) == 0) return false;
        this->value = static_cast<T>(enum_keys_map[str]);
        return true;
    };

    static t_config_enum_values get_enum_values();  //������ľ�̬�������÷�����
};

/* We use this one in DynamicConfig objects, otherwise it's better to use
   the specialized ConfigOptionEnum<T> containers. */
class ConfigOptionEnumGeneric : public ConfigOption   //����ѡ��ö�ٷ���
{
    public:
    int value;
    t_config_enum_values* keys_map;

    operator int() const { return this->value; };

    std::string serialize() const {
        for (t_config_enum_values::iterator it = this->keys_map->begin(); it != this->keys_map->end(); ++it) {
            if (it->second == this->value) return it->first;
        }
        return "";
    };

    bool deserialize(std::string str) {
        if (this->keys_map->count(str) == 0) return false;
        this->value = (*this->keys_map)[str];
        return true;
    };
};

enum ConfigOptionType {
    coFloat,
    coFloats,
    coInt,
    coInts,
    coString,
    coStrings,
    coPercent,
    coFloatOrPercent,
    coPoint,
    coPoints,
    coBool,
    coBools,
    coEnum,
};

class ConfigOptionDef   //����ѡ���
{
    public:
    ConfigOptionType type;
    std::string gui_type;
    std::string gui_flags;
    std::string label;
    std::string full_label;
    std::string category;
    std::string tooltip;
    std::string sidetext;
    std::string cli;
    t_config_option_key ratio_over;   //����ѡ��������ַ���
    bool multiline;
    bool full_width;
    bool readonly;
    int height;
    int width;
    int min;
    int max;
    std::vector<t_config_option_key> aliases;
    std::vector<t_config_option_key> shortcut;
    std::vector<std::string> enum_values;
    std::vector<std::string> enum_labels;
    t_config_enum_values enum_keys_map;

    ConfigOptionDef() : multiline(false), full_width(false), readonly(false),
                        height(-1), width(-1), min(INT_MIN), max(INT_MAX) {};
};

typedef std::map<t_config_option_key,ConfigOptionDef> t_optiondef_map;

class ConfigBase   //���û���
{
    public:
    t_optiondef_map* def;

    ConfigBase() : def(NULL) {};
    bool has(const t_config_option_key opt_key);
    virtual ConfigOption* option(const t_config_option_key opt_key, bool create = false) = 0;
    virtual const ConfigOption* option(const t_config_option_key opt_key) const = 0;
    virtual t_config_option_keys keys() const = 0;
    void apply(const ConfigBase &other, bool ignore_nonexistent = false);
    bool equals(ConfigBase &other);
    t_config_option_keys diff(ConfigBase &other);
    std::string serialize(const t_config_option_key opt_key);
    bool set_deserialize(const t_config_option_key opt_key, std::string str);
    //void set_ifndef(t_config_option_key opt_key, SV* value, bool deserialize = false);  //SV������ʲô����
    double get_abs_value(const t_config_option_key opt_key);
    double get_abs_value(const t_config_option_key opt_key, double ratio_over);
    void setenv_();

    #ifdef SLIC3RXS
    SV* as_hash();
    SV* get(t_config_option_key opt_key);
    SV* get_at(t_config_option_key opt_key, size_t i);
    bool set(t_config_option_key opt_key, SV* value);
    bool set_deserialize(const t_config_option_key opt_key, SV* str);
    #endif
};

class DynamicConfig : public ConfigBase   //��̬����
{
    public:
    DynamicConfig() {};
    DynamicConfig(const DynamicConfig& other);
    DynamicConfig& operator= (DynamicConfig other);
    void swap(DynamicConfig &other);
    ~DynamicConfig();
    template<class T> T* opt(const t_config_option_key opt_key, bool create = false);
    ConfigOption* option(const t_config_option_key opt_key, bool create = false);
    const ConfigOption* option(const t_config_option_key opt_key) const;
    t_config_option_keys keys() const;
    void erase(const t_config_option_key opt_key);

    private:
    typedef std::map<t_config_option_key,ConfigOption*> t_options_map;
    t_options_map options;
};

class StaticConfig : public ConfigBase    //��̬����
{
    public:
    t_config_option_keys keys() const;
    virtual ConfigOption* option(const t_config_option_key opt_key, bool create = false) = 0;
    const ConfigOption* option(const t_config_option_key opt_key) const;

    #ifdef SLIC3RXS
    bool set(t_config_option_key opt_key, SV* value);
    #endif
};

}

#endif // CONFIG_H

