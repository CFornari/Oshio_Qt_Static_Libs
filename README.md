<h1> Oshio_Qt_Static_Libs </h1>

A set of generic static libraries which I use in my projects and want to share with the public.

Every library has a QT Test project associate with it, run those to see if your enviorment has the correct setup.

<h3> Running QT Test</h3>
<ol>
<li>Clone the project.</li>
<li>Open the .pro in QtCreator.</li>
<li>Build All: Ctrl+Shift+B</li>
<li>Run all tests: Tools -->  Tests --> Run All Tests</li>
</ol>

<h2> Status </h2>
Currently using Qt 5.13.0 <br>
Just SQLiteSave is correct. (building in Ubuntu 18.04)<br>
EspUdp is not building and the tests are incomplete.<br>

<h2> Modules </h2>

<h3> Database Module </h3>
Interfaces to manipulate database querys.

<h4>SQLiteSave</h4> 
Implements a simple interface to create tables, columns and add rows to a SQLite database.
