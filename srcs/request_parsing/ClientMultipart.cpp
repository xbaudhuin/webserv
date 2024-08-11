#include "Client.hpp"

bool Client::getLineFromBuffer(std::string &line, bool headerDone) {
  int i = 0;
  line = "";
  i = hasNewLine();
  if (i == -1) {
    if (headerDone == true) {
      if (_vBuffer.size() < _boundaryMaxSize) {
        return (false);
      } else {
        line.insert(line.begin(), _vBuffer.begin(),
                    _vBuffer.end() - _boundaryMaxSize);
        _vBuffer.erase(_vBuffer.begin(), _vBuffer.end() - _boundaryMaxSize);
        return (false);
      }
    } else {
      if (_vBuffer.size() < _headerMaxSize) {
        return (false);
      } else {
        _statusCode = 422;
        logErrorClient("Client::getLineFromBuffer: no newline in header");
        return (true);
      }
    }
  }
  if (static_cast<size_t>(i) < _vBuffer.size())
    i++;
  line.insert(line.begin(), _vBuffer.begin(), _vBuffer.begin() + i);
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i);
  return (true);
}

bool Client::checkBoundary(void) {
  if (_vBuffer.size() < _boundary.size())
    return (false);
  std::string tmp(_vBuffer.begin(), _vBuffer.begin() + _boundary.size());
  if (tmp.compare(_boundary) == 0)
    return (true);
  return (false);
}

bool Client::checkEndBoundary(multipartRequest &multi) {
  if (_vBuffer.size() < _boundary.size() + 2)
    return (false);
  std::string tmp(_vBuffer.begin(), _vBuffer.begin() + _boundary.size() + 2);
  if (tmp.compare(0, _boundary.size(), _boundary) == 0) {
    multi.isDone = true;
    _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + _boundary.size());
    if (tmp.substr(_boundary.size(), 2) == "--") {
      _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + 2);
      removeTrailingLineFromBuffer();
      _currentMultipart = 0;
      return (true);
    }
    removeTrailingLineFromBuffer();
  }
  if (_statusCode >= 400) {
    return (true);
  }
  return (false);
}

std::string Client::getBoundaryString(std::string &boundaryHeader) {
  size_t pos;
  pos = boundaryHeader.find(';');
  if (pos == boundaryHeader.npos) {
    _statusCode = 400;
    logErrorClient("Client::getBoundaryString: invalid boundary header");
    return ("");
  }
  pos++;
  if (boundaryHeader[pos] == ' ')
    pos++;
  if (pos + 11 >= boundaryHeader.size() ||
      boundaryHeader.substr(pos, 11) != "boundary=--") {
    _statusCode = 400;
    logErrorClient("Client::getBoundaryString: invalid boundary header");
    return ("");
  }
  return ("----" + boundaryHeader.substr(pos + 11));
}

bool Client::checkHeaderMulti(multipartRequest &multi) {
  if (_statusCode >= 400)
    return (true);
  std::map<std::string, std::string>::iterator it;
  it = multi.header.find("content-type");
  if (it == multi.header.end()) {
    _statusCode = 400;
    logErrorClient("Client::checkHeaderMulti: no content-type header");
    return (true);
  }
  it = multi.header.find("content-disposition");
  if (it == multi.header.end()) {
    _statusCode = 400;
    logErrorClient("Client::checkHeaderMulti: no content-disposition header");
    return (true);
  }
  std::string tmp = (*it).second;
  size_t pos = tmp.find("name=\"");
  if (pos == tmp.npos || pos + 7 >= tmp.size()) {
    _statusCode = 422;
    logErrorClient(
        "Client::checkHeaderMulti: invalid content-disposition header");
    return (true);
  }
  pos += 7;
  pos = tmp.find_first_of('\"');
  if (pos == tmp.npos) {
    _statusCode = 422;
    logErrorClient(
        "Client::checkHeaderMulti: invalid content-disposition header");
    return (true);
  }
  pos = tmp.find("filename=\"", pos);
  if (pos == tmp.npos) {
    _statusCode = 422;
    logErrorClient(
        "Client::checkHeaderMulti: invalid content-disposition header");
    return (true);
  }
  pos += 10;
  size_t end = pos;
  end = tmp.find_first_of('\"', end);
  if (end == tmp.npos) {
    _statusCode = 422;
    logErrorClient(
        "Client::checkHeaderMulti: invalid content-disposition header");
    return (true);
  }
  if (_location->getUploadLocation() == "")
    multi.file = _location->getRoot();
  else
    multi.file = "." + _location->getUploadLocation();
  multi.file += tmp.substr(pos, end - pos);
  multi.headerDone = true;
  return (true);
}

bool Client::getHeaderMulti(multipartRequest &multi) {
  std::string line;
  if (getLineFromBuffer(line, multi.headerDone) == false)
    return (false);
  while (line.empty() == false && _statusCode < 400) {
    removeReturnCarriageNewLine(line);
    if (line == "")
      break;
    insertInMap(line, multi.header);
    if (_vBuffer.size() < _boundary.size())
      return (false);
    if (getLineFromBuffer(line, multi.headerDone) == false)
      return (false);
  }
  return (checkHeaderMulti(multi));
}

bool Client::saveMultiToTmpfile(multipartRequest &multi) {
  if (multi.body.size() == 0)
    return (true);
  if (multi.tmpFilename == "") {
    std::stringstream ss;
    ss << _multipart.size();
    multi.tmpFilename = "webserv_tmpmulti" + ss.str();
    ss.str("");
    ss.clear();
    ss << getTime();
    multi.tmpFilename += "id" + ss.str();
    _tmpFd = open(multi.tmpFilename.c_str(),
                  O_RDWR | O_CLOEXEC | O_CREAT | O_TRUNC, 00644);
    if (_tmpFd == -1) {
      _statusCode = 500;
      logErrorClient("Client::saveMultiToTmpFile: fail to open: " +
                     multi.tmpFilename);
      return (false);
    }
  }
  bool ret = saveToTmpFile(multi.body);
  resetVector(multi.body);
  return (ret);
}

bool Client::checkBodyMultipartCgi(std::string &boundary) {
  _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.end());
  std::string tmp(_vBuffer.begin(), _vBuffer.end());
  _sizeChunk += _vBuffer.size();
  if (_location && _sizeChunk >= _location->getLimitBodySize() &&
      _location->getLimitBodySize() != 0) {
    _statusCode = 413;
    return (true);
  }
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.end());
  saveToTmpFile();
  size_t pos = tmp.rfind(boundary + "--");
  if (pos == tmp.npos)
    return (false);
  else if (pos != tmp.npos)
    return (true);
  return (false);
}

bool Client::getMultipartBody(multipartRequest &multi) {
  std::string line;
  std::string next;
  while (true) {
    if (_vBuffer.size() < _boundary.size()) {
      break;
    }
    if (getLineFromBuffer(line, multi.headerDone) == false) {
      multi.body.insert(multi.body.end(), line.begin(), line.end());
      saveMultiToTmpfile(multi);
      return (false);
    }
    if (line == "") {
      multi.body.insert(multi.body.end(), _vBuffer.begin(),
                        _vBuffer.begin() + 1);
      _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + 1);
    }
    if (checkBoundary() == true) {
      removeReturnCarriageNewLine(line);
      multi.body.insert(multi.body.end(), line.begin(), line.end());
      multi.isDone = true;
      break;
    }
    //
    else
      multi.body.insert(multi.body.end(), line.begin(), line.end());
  }
  if (multi.body.size() != 0) {
    checkBodyHeader(multi, multi.body);
    saveMultiToTmpfile(multi);
    if (multi.isDone == true) {
      close(_tmpFd);
    }
  }
  if (checkEndBoundary(multi) == true) {
    if (_vBuffer.size() > 0) {
      _statusCode = 400;
      logErrorClient("Client::getMultipartBody: body after end boundary");
    } else
      _statusCode = 201;
    return (true);
  }
  return (false);
}

bool Client::parseMultipartRequest() {
  if (_location && _location->isCgi(_sUri) == true)
    return (checkBodyMultipartCgi(_boundary));
  if (_vBuffer.size() < _boundary.size())
    return (false);
  if (_checkMulti == false) {
    if (checkBoundary() == false) {
      if (_vBuffer.size() >= _boundary.size()) {
        _statusCode = 400;
        logErrorClient("Client::parseMultipartRequest: no boundary string");
        return (true);
      } else
        return (false);
    }
    _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + _boundary.size());
    removeTrailingLineFromBuffer();
    if (_statusCode >= 400) {
      return (true);
    }
    _checkMulti = true;
  }
  if (_multipart.size() == 0 || _multipart.back().isDone == true) {
    multipartRequest ref;
    ref.isDone = false;
    ref.headerDone = false;
    _multipart.push_back(ref);
  }
  multipartRequest &multi = _multipart.back();
  if (multi.headerDone == false) {
    bool newLine = getHeaderMulti(multi);
    if (newLine == false)
      return (false);
    if (_statusCode >= 400) {
      return (true);
    }
  }
  if (_vBuffer.size() < _boundary.size())
    return (false);
  bool end = getMultipartBody(multi);
  if (multi.tmpFilename == "" &&
      (_vBuffer.size() > _boundary.size() || end == true)) {
    _multipart.pop_back();
  } else if (multi.isDone == false)
    return (false);
  if (end == true)
    return (true);
  return (parseMultipartRequest());
}
