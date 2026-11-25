#include <cstring>
#include <cstdio>
#include "Tilc/Apps/Www/WwwApp.h"
#include "Tilc/Commerce/Shop.h"

#include "CommonRequestHandlers.h"
#include "Tilc/Apps/Www/RequestHandler.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Utils/Time.h"

std::string Tilc::Apps::Www::DbHost{"localhost"};
std::string Tilc::Apps::Www::DbName{"teacher"};
std::string Tilc::Apps::Www::DbUser{"teacher"};
std::string Tilc::Apps::Www::DbPasswd{"teacher"};
Tilc::Apps::Www::TRequestHandler* Tilc::Apps::Www::rh{};
Tilc::Apps::Www::TWwwApp* Tilc::Apps::Www::Application{};

Tilc::Apps::Www::ERequestMethod Tilc::Apps::Www::ToRequestMethod(std::string method)
{
    if (method == "GET")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_GET;
    }
    else if(method == "POST")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_POST;
    }
    else if(method == "HEAD")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_HEAD;
    }
    else if(method == "PUT")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_PUT;
    }
    else if(method == "DELETE")
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_DELETE;
    }
    else
    {
        return Tilc::Apps::Www::ERequestMethod::ERM_INVALID;
    }
}

std::string Tilc::Apps::Www::urlencode(const std::string& Input)
{
	std::string Result;
	char	 ch;
	size_t	 sz, cur;
	int	 rc, cp;
    char* p;

	if (Input.empty())
		return "";

	/* 
	 * Leave three bytes per input byte for encoding. 
	 * This ensures we needn't range-check.
	 * First check whether our size overflows. 
	 * We do this here because we need our size!
	 */
	sz = Input.length() + 1;
    Result.reserve(sz * 3);

	for (cur = 0, cp = 0; (ch = Input[cp]) != '\0'; ++cp)
    {
		if (std::isalnum((unsigned char)ch) || ch == '-' || 
		    ch == '_' || ch == '.' || ch == '~')
        {
			Result += ch;
			continue;
		}
        else if (' ' == ch)
        {
			Result += '+';
			continue;
		}
		rc = std::snprintf(p + cur, 4, "%%%.2hhX", (unsigned char)ch);
		if (rc != 3)
        {
			return "";
		}
		cur += 3;
	}

	return Result;
}

std::string Tilc::Apps::Www::urldecode(const std::string& Input)
{
	char	 	 c, d;
	const char	*tail;
    std::string Result;
    int p;

	if (Input.empty())
		return "";

	/*
	 * Keep track of two positions: "p", where we'll write the
	 * decoded results, and "tail", which is from where we'll
	 * decode hex or copy data.
	 */
    p = 0;
	for (tail = Input.c_str(); (c = *tail) != '\0'; Result += c, ++p)
    {
		if (c != '%')
        {
			if (c == '+')
				c = ' ';
			tail++;
			continue;
		}

		/* 
		 * Read hex '%xy' as two unsigned chars "c" and "d" then
		 * combine them back into "c".
		 */
#ifdef __linux__
		if (std::sscanf(tail + 1, "%1hhx%1hhx", &d, &c) != 2 || (c |= d << 4) == '\0')
#elif __WIN32
        if (std::sscanf_s(tail + 1, "%1hhx%1hhx", &d, &c) != 2 || (c |= d << 4) == '\0')
#endif
        {
			return "[ERROR!!!]: malformed percent-encoded sequence";
		}
		tail += 3;
	}

	return Result;
}

Tilc::Apps::Www::TWwwApp::TWwwApp(const Tilc::TExtString& AllowedRootDir, const Tilc::TExtString& AppName, const Tilc::TExtString& AppSlug, const Tilc::TExtString& AllowOrigin)
    : m_AllowedRootDir(AllowedRootDir), m_AppName(AppName), m_AppSlug(AppSlug), m_AllowOrigin(AllowOrigin)
{
	m_TemplateDir = m_AllowedRootDir + "/public_html/templates/" + m_TemplateName;
}

Tilc::Apps::Www::TWwwApp::~TWwwApp()
{
}

void Tilc::Apps::Www::TWwwApp::AddAvailableLanguages(const std::initializer_list<const char*>& Languages)
{
	std::for_each(Languages.begin(), Languages.end(), [this](Tilc::TExtString lang) {
		m_Languages.push_back(lang);
	});
}

void Tilc::Apps::Www::TWwwApp::Index(TRequestHandler* rh)
{
	Tilc::TExtString Content, CategoriesBox, Product;

	{
		Tilc::TFile FileIndex((m_TemplateDir + "/index.html").c_str());
		if (FileIndex.m_File)
		{
			FileIndex.ReadContent(Content);
		}
	}

	{
		Tilc::TFile FileCategoriesBox((m_TemplateDir + "/category-box.html").c_str());
		if (FileCategoriesBox.m_File)
		{
			FileCategoriesBox.ReadContent(CategoriesBox);
		}
	}

	{
		Tilc::TFile FileCategoriesBoxProduct((m_TemplateDir + "/category-box-product.html").c_str());
		if (FileCategoriesBoxProduct.m_File)
		{
			FileCategoriesBoxProduct.ReadContent(Product);
		}
	}

	Tilc::Commerce::TCategory c;

	Tilc::Data::TDBDataRows DataCategories;
	Tilc::Apps::Www::DB->Select("SELECT id, name, slug FROM categories ORDER BY id ASC", DataCategories);

	Tilc::Data::TDBDataRows DataProducts;
	Tilc::Apps::Www::DB->Select("SELECT p.id, pc.category_id cid, p.name, p.slug, p.short_description FROM products p LEFT JOIN products_categories pc ON (pc.product_id=p.id) ORDER BY category_id ASC, name ASC;", DataProducts);

	size_t Pos = Content.find("{content}");
	(*rh) << Content.substr(0, Pos);
	size_t ProdIndex = 0;
	for (size_t i = 0; i < DataCategories.size(); ++i)
	{
		Tilc::TExtString CatBox = CategoriesBox;
		Tilc::TExtString ProductsList;
		CatBox.StrReplace("{category_name}", Tilc::Unescape(DataCategories[i][1]));
		for (size_t j = ProdIndex; j < DataProducts.size(); ++j)
		{
			if (DataProducts[j][1] != DataCategories[i][0]) break;
			Tilc::TExtString Prod = Product;
			Prod.StrReplace("{product_name}", Tilc::Unescape(DataProducts[j][2]));
			Prod.StrReplace("{product_slug}", DataProducts[j][3]);
			Prod.StrReplace("{product_short_description}", Tilc::Unescape(DataProducts[j][4]));

			ProductsList += Prod;

			++ProdIndex;
		}
		CatBox.StrReplace("{products_in_category}", ProductsList);
		(*rh) << CatBox;
	}
	(*rh) << Content.substr(Pos+9);
}
