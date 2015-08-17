// Generated by CoffeeScript 1.6.3
(function() {
  var Deinflector, rules;

  Deinflector = (typeof exports !== "undefined" && exports !== null) && exports || (this.Deinflector = {});

  Deinflector.deinflect = function(verb) {
    var beginning, ending, form, forms, k, len, match, v, _i, _len, _ref, _ref1;
    forms = [verb];
    _ref = (function() {
      var _results;
      _results = [];
      for (k in rules) {
        v = rules[k];
        if (k <= verb.length) {
          _results.push(k);
        }
      }
      return _results;
    })();
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      len = _ref[_i];
      beginning = verb.substr(0, verb.length - len);
      ending = verb.substr(-len);
      _ref1 = rules[len];
      for (match in _ref1) {
        form = _ref1[match];
        if (ending === match) {
          forms.push("" + beginning + form);
        }
      }
    }
    return forms;
  };

  rules = {
    9: {
      "くありませんでした": "い"
    },
    7: {
      "いませんでした": "う",
      "きませんでした": "く",
      "きませんでした": "くる",
      "ぎませんでした": "ぐ",
      "しませんでした": "す",
      "しませんでした": "する",
      "ちませんでした": "つ",
      "にませんでした": "ぬ",
      "びませんでした": "ぶ",
      "みませんでした": "む",
      "りませんでした": "る"
    },
    6: {
      "くありません": "い",
      "ませんでした": "る"
    },
    5: {
      "いましょう": "う",
      "きましょう": "く",
      "きましょう": "くる",
      "ぎましょう": "ぐ",
      "しましょう": "す",
      "しましょう": "する",
      "ちましょう": "つ",
      "にましょう": "ぬ",
      "びましょう": "ぶ",
      "みましょう": "む",
      "りましょう": "る"
    },
    4: {
      "いじゃう": "ぐ",
      "いすぎる": "う",
      "いちゃう": "く",
      "いなさい": "う",
      "いました": "う",
      "いません": "う",
      "かったら": "い",
      "かったり": "い",
      "きすぎる": "く",
      "きすぎる": "くる",
      "ぎすぎる": "ぐ",
      "きちゃう": "くる",
      "きなさい": "く",
      "きなさい": "くる",
      "ぎなさい": "ぐ",
      "きました": "く",
      "きました": "くる",
      "ぎました": "ぐ",
      "きません": "く",
      "きません": "くる",
      "ぎません": "ぐ",
      "こさせる": "くる",
      "こられる": "くる",
      "しすぎる": "す",
      "しすぎる": "する",
      "しちゃう": "す",
      "しちゃう": "する",
      "しなさい": "す",
      "しなさい": "する",
      "しました": "す",
      "しました": "する",
      "しません": "す",
      "しません": "する",
      "ちすぎる": "つ",
      "ちなさい": "つ",
      "ちました": "つ",
      "ちません": "つ",
      "っちゃう": "う",
      "っちゃう": "く",
      "っちゃう": "つ",
      "っちゃう": "る",
      "にすぎる": "ぬ",
      "になさい": "ぬ",
      "にました": "ぬ",
      "にません": "ぬ",
      "びすぎる": "ぶ",
      "びなさい": "ぶ",
      "びました": "ぶ",
      "びません": "ぶ",
      "ましょう": "る",
      "みすぎる": "む",
      "みなさい": "む",
      "みました": "む",
      "みません": "む",
      "りすぎる": "る",
      "りなさい": "る",
      "りました": "る",
      "りません": "る",
      "んじゃう": "ぬ",
      "んじゃう": "ぶ",
      "んじゃう": "む"
    },
    3: {
      "いそう": "う",
      "いたい": "う",
      "いたら": "く",
      "いだら": "ぐ",
      "いたり": "く",
      "いだり": "ぐ",
      "います": "う",
      "かせる": "く",
      "がせる": "ぐ",
      "かった": "い",
      "かない": "く",
      "がない": "ぐ",
      "かれる": "く",
      "がれる": "ぐ",
      "きそう": "く",
      "きそう": "くる",
      "ぎそう": "ぐ",
      "きたい": "く",
      "きたい": "くる",
      "ぎたい": "ぐ",
      "きたら": "くる",
      "きたり": "くる",
      "きます": "く",
      "きます": "くる",
      "ぎます": "ぐ",
      "くない": "い",
      "ければ": "い",
      "こない": "くる",
      "こよう": "くる",
      "これる": "くる",
      "させる": "する",
      "させる": "る",
      "さない": "す",
      "される": "す",
      "される": "する",
      "しそう": "す",
      "しそう": "する",
      "したい": "す",
      "したい": "する",
      "したら": "す",
      "したら": "する",
      "したり": "す",
      "したり": "する",
      "しない": "する",
      "します": "す",
      "します": "する",
      "しよう": "する",
      "すぎる": "い",
      "すぎる": "る",
      "たせる": "つ",
      "たない": "つ",
      "たれる": "つ",
      "ちそう": "つ",
      "ちたい": "つ",
      "ちます": "つ",
      "ちゃう": "る",
      "ったら": "う",
      "ったら": "つ",
      "ったら": "る",
      "ったり": "う",
      "ったり": "つ",
      "ったり": "る",
      "なさい": "る",
      "なせる": "ぬ",
      "なない": "ぬ",
      "なれる": "ぬ",
      "にそう": "ぬ",
      "にたい": "ぬ",
      "にます": "ぬ",
      "ばせる": "ぶ",
      "ばない": "ぶ",
      "ばれる": "ぶ",
      "びそう": "ぶ",
      "びたい": "ぶ",
      "びます": "ぶ",
      "ました": "る",
      "ませる": "む",
      "ません": "る",
      "まない": "む",
      "まれる": "む",
      "みそう": "む",
      "みたい": "む",
      "みます": "む",
      "らせる": "る",
      "らない": "る",
      "られる": "る",
      "りそう": "る",
      "りたい": "る",
      "ります": "る",
      "わせる": "う",
      "わない": "う",
      "われる": "う",
      "んだら": "ぬ",
      "んだら": "ぶ",
      "んだら": "む",
      "んだり": "ぬ",
      "んだり": "ぶ",
      "んだり": "む"
    },
    2: {
      "いた": "く",
      "いだ": "ぐ",
      "いて": "く",
      "いで": "ぐ",
      "えば": "う",
      "える": "う",
      "おう": "う",
      "かず": "く",
      "がず": "ぐ",
      "きた": "くる",
      "きて": "くる",
      "くて": "い",
      "けば": "く",
      "げば": "ぐ",
      "ける": "く",
      "げる": "ぐ",
      "こい": "くる",
      "こう": "く",
      "ごう": "ぐ",
      "こず": "くる",
      "さず": "す",
      "した": "す",
      "した": "する",
      "して": "す",
      "して": "する",
      "しろ": "する",
      "せず": "する",
      "せば": "す",
      "せよ": "する",
      "せる": "す",
      "そう": "い",
      "そう": "す",
      "そう": "る",
      "たい": "る",
      "たず": "つ",
      "たら": "る",
      "たり": "る",
      "った": "う",
      "った": "く",
      "った": "つ",
      "った": "る",
      "って": "う",
      "って": "く",
      "って": "つ",
      "って": "る",
      "てば": "つ",
      "てる": "つ",
      "とう": "つ",
      "ない": "る",
      "なず": "ぬ",
      "ねば": "ぬ",
      "ねる": "ぬ",
      "のう": "ぬ",
      "ばず": "ぶ",
      "べば": "ぶ",
      "べる": "ぶ",
      "ぼう": "ぶ",
      "ます": "る",
      "まず": "む",
      "めば": "む",
      "める": "む",
      "もう": "む",
      "よう": "る",
      "らず": "る",
      "れば": "る",
      "れる": "る",
      "ろう": "る",
      "わず": "う",
      "んだ": "ぬ",
      "んだ": "ぶ",
      "んだ": "む",
      "んで": "ぬ",
      "んで": "ぶ",
      "んで": "む"
    },
    1: {
      "い": "いる",
      "い": "う",
      "い": "る",
      "え": "う",
      "え": "える",
      "き": "きる",
      "き": "く",
      "ぎ": "ぎる",
      "ぎ": "ぐ",
      "く": "い",
      "け": "く",
      "け": "ける",
      "げ": "ぐ",
      "げ": "げる",
      "さ": "い",
      "し": "す",
      "じ": "じる",
      "ず": "る",
      "せ": "す",
      "せ": "せる",
      "ぜ": "ぜる",
      "た": "る",
      "ち": "ちる",
      "ち": "つ",
      "て": "つ",
      "て": "てる",
      "て": "る",
      "で": "でる",
      "な": "",
      "に": "にる",
      "に": "ぬ",
      "ね": "ぬ",
      "ね": "ねる",
      "ひ": "ひる",
      "び": "びる",
      "び": "ぶ",
      "へ": "へる",
      "べ": "ぶ",
      "べ": "べる",
      "み": "みる",
      "み": "む",
      "め": "む",
      "め": "める",
      "よ": "る",
      "り": "りる",
      "り": "る",
      "れ": "る",
      "れ": "れる",
      "ろ": "る"
    }
  };

}).call(this);
