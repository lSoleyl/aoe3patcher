#include "pch.hpp"
#include "ProtoPatcher.hpp"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOM.hpp>


using namespace xercesc;
using namespace std;

      /* name -> limit */
std::map<wstring, wstring> buildLimits = { 
  {L"House", L"20"}, 
  {L"Manor", L"20"},  
  {L"HouseEast", L"20"},
  {L"HouseMed", L"20"},
  {L"Tipi", L"20"},
  {L"Longhouse", L"14"},
  {L"HouseAztec", L"20"}
};

struct ProtoPatcher::Internals {
  xercesc::DOMDocument* domDoc;
  XercesDOMParser* parser;
  HandlerBase* errHandler;

  ~Internals() {
    delete parser;
    delete errHandler;
  }
};

std::string convert(const std::wstring& source) {
  return string(source.begin(), source.end());
}

DOMElement* FindChild(DOMElement* parent, const wstring& tagName) {
  for(DOMElement* current = parent->getFirstElementChild(); current; current = current->getNextElementSibling()) {
    if (tagName == current->getTagName())
      return current;
  }
  return nullptr;
}

DOMElement* FindChild(DOMElement* parent, const wstring& tagName, const wstring& attributeName, const wstring& attributeValue) {
  for(DOMElement* current = parent->getFirstElementChild(); current; current = current->getNextElementSibling()) {
    if (tagName == current->getTagName() && attributeValue == current->getAttribute(attributeName.c_str()))
      return current;
  }
  return nullptr;
}


ProtoPatcher::~ProtoPatcher() {
  delete data;
}

ProtoPatcher::ProtoPatcher() : data(new Internals) {
  auto parser = data->parser = new XercesDOMParser;
  
  data->errHandler = new HandlerBase;
  parser->setErrorHandler(data->errHandler);

  try {
    parser->parse(".\\data\\protox.xml");
  } catch (const XMLException& e) {
    throw std::exception(("XMLException occurred while parsing protox.xml" + convert(e.getMessage())).c_str());
  } catch (const DOMException& e) {
    throw std::exception(("DOMException occurred while parsing protox.xml: " + convert(e.getMessage())).c_str());
  } catch (...) {
    throw std::exception("Caught unknown exception in parser->parse()");
  }

  data->domDoc = parser->getDocument();

  if (!data->domDoc)
    throw std::exception("Error, while parsing data\\protox.xml... file might be missing");
}

bool ProtoPatcher::HasLimits() const {
  return std::all_of(buildLimits.begin(), buildLimits.end(), [this](std::pair<wstring, wstring> entry) {
    auto root = data->domDoc->getDocumentElement();

    auto unitElement = FindChild(root, L"Unit", L"name", entry.first);

    if (unitElement) {
      auto limit = FindChild(unitElement, L"BuildLimit");
      return limit && limit->getTextContent() == entry.second;
    }

    return false;
  });
}

void ProtoPatcher::RemoveLimits() {
  auto root = data->domDoc->getDocumentElement();
  for(auto& entry : buildLimits) {
    auto unitElement = FindChild(root, L"Unit", L"name", entry.first);
    if (!unitElement) {
      cerr << "[WARN] missing unit in protox.xml: '" << convert(entry.first) << "'" << endl;
      continue;
    }

    auto limit = FindChild(unitElement, L"BuildLimit");
    if (limit) {
      unitElement->removeChild(limit);
      limit->release();
    }
  }

  SaveToFile();
}

void ProtoPatcher::RestoreLimits() {
  auto root = data->domDoc->getDocumentElement();
  for(auto& entry : buildLimits) {
    auto unitElement = FindChild(root, L"Unit", L"name", entry.first);
    if (!unitElement) {
      cerr << "[WARN] missing unit in protox.xml: '" << convert(entry.first) << "'" << endl;
      continue;
    }

    auto limit = FindChild(unitElement, L"BuildLimit");
    if (!limit) {
      limit = data->domDoc->createElement(L"BuildLimit");
      limit->setTextContent(entry.second.c_str());
      unitElement->appendChild(limit);
    }
  }

  SaveToFile();
}

void ProtoPatcher::SaveToFile() {
  DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(L"LS");
  DOMLSSerializer* theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();

  if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
      theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

  theSerializer->writeToURI(data->domDoc, L".\\data\\protox.xml");
  theSerializer->release();
}