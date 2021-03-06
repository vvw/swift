// Generated by CoffeeScript 1.6.3
(function() {
  var flatten, readDataFile;

  window.Dictionary = (function() {
    function Dictionary() {
      console.log("Create dictionary instance");
    }

    Dictionary.prototype.find = function(word, limit) {
      var idx, l, longest, result, results, w, _i, _len, _ref;
      if (!this.dict) {
        return [];
      }
      this.cachedWord = {};
      longest = null;
      results = [];
      _ref = (function() {
        var _j, _ref, _results;
        _results = [];
        for (l = _j = _ref = word.length; _ref <= 1 ? _j <= 1 : _j >= 1; l = _ref <= 1 ? ++_j : --_j) {
          _results.push(word.substring(0, l));
        }
        return _results;
      })();
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        w = _ref[_i];
        result = this.searchWord(w);
        if (result.length > 0) {
          longest || (longest = w);
        }
        results.push.apply(results, result);
      }
      return {
        results: (function() {
          var _j, _len1, _results;
          _results = [];
          for (idx = _j = 0, _len1 = results.length; _j < _len1; idx = ++_j) {
            result = results[idx];
            if (idx < limit) {
              _results.push(result);
            }
          }
          return _results;
        })(),
        match: longest
      };
    };

    Dictionary.prototype.pushWordToResults = function(results, word, matchedWord) {
      var item, parsed, pending, record, _i, _len, _results;
      if (matchedWord == null) {
        matchedWord = null;
      }
      if (!this.cachedWord[word]) {
        this.cachedWord[word] = true;
        if (record = this.dict.words[word]) {
          parsed = (function() {
            var _i, _len, _results;
            _results = [];
            for (_i = 0, _len = record.length; _i < _len; _i++) {
              item = record[_i];
              _results.push(this.parseResult(word, item));
            }
            return _results;
          }).call(this);
          _results = [];
          for (_i = 0, _len = parsed.length; _i < _len; _i++) {
            pending = parsed[_i];
            if ((!matchedWord) || (pending.kana === matchedWord || pending.kanji === matchedWord)) {
              _results.push(results.push(pending));
            }
          }
          return _results;
        }
      }
    };

    Dictionary.prototype.searchWord = function(word) {
      var hiragana, index, indexes, results, variant, variants, _i, _j, _k, _len, _len1, _len2;
      results = [];
      variants = word.length > 1 ? Deinflector.deinflect(word) : [word];
      hiragana = Romaji.toHiragana(word).join("");
      if (hiragana.length > 0) {
        variants.push(hiragana);
      }
      for (_i = 0, _len = variants.length; _i < _len; _i++) {
        variant = variants[_i];
        this.pushWordToResults(results, variant);
      }
      for (_j = 0, _len1 = variants.length; _j < _len1; _j++) {
        variant = variants[_j];
        if (indexes = this.dict.indexes[variant]) {
          for (_k = 0, _len2 = indexes.length; _k < _len2; _k++) {
            index = indexes[_k];
            this.pushWordToResults(results, index, variant);
          }
        }
      }
      return results;
    };

    Dictionary.prototype.parseResult = function(kanji, result) {
      var kana, parts, translation;
      if (result[0] === "[") {
        parts = result.split(/[\[\]]/);
        kana = parts[1];
        translation = parts[2].substring(1);
      } else {
        kana = kanji;
        translation = result;
      }
      translation = translation.replace(/^\/\(\S+\) /, "");
      translation = translation.replace(/\(P\)\/$/, "");
      translation = translation.slice(0, -1).split("/").join("; ");
      return {
        kana: kana,
        kanji: kanji,
        translation: translation,
        romaji: Romaji.toRomaji(kana)
      };
    };

    Dictionary.prototype.load = function() {
      var _this = this;
      return readDataFile("dictionary.js", function(data) {
        eval(data);
        return _this.dict = loadedDict;
      });
    };

    Dictionary.prototype.unload = function() {
      return this.dict = null;
    };

    return Dictionary;

  })();

  flatten = function(array) {
    var element, flattened, _i, _len;
    flattened = [];
    for (_i = 0, _len = array.length; _i < _len; _i++) {
      element = array[_i];
      if (element instanceof Array) {
        flattened.push.apply(flattened, flatten(element));
      } else {
        flattened.push(element);
      }
    }
    return flattened;
  };

  readDataFile = function(file, success) {
    var req;
    req = new XMLHttpRequest();
    req.open("GET", safari.extension.baseURI + "data/" + file, true);
    req.onload = function(e) {
      if (req.readyState === 4) {
        return success(req.responseText);
      }
    };
    return req.send(null);
  };

}).call(this);
