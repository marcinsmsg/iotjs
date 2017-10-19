/* Copyright 2017-present Samsung Electronics Co., Ltd. and other contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

var assert = require('assert');
var console = require('console');
var fs = require('fs');

var testdir = process.cwd() + "/resources";
var flags = "r";

console.warn('Opening directory ' + testdir)
// fstat - directory
fs.open(testdir, flags, function(err, fd) {
  if (err) {
    console.warn('dir open failed, ' + err.message);
    throw err;
  }
  console.warn('dir open OK, fstat\'ing');
  fs.fstat(fd, function(err, stat) {
    if (err) {
      console.warn('dir fstat failed, ' + err.message);
      throw err;
    }
    console.warn('verifying dir fstat results');
    assert.equal(stat.isFile(), false);
    assert.equal(stat.isDirectory(), true);

    console.warn('closing directory');
    fs.close(fd, function(err) {
      if (err) {
        console.warn('dir close failed, ' + err.message);
        throw err;
      }
    });
  });
});