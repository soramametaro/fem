概要：
stl形式の物体表面のデータを用いて，物体の内部までメッシュで表現するボリュームメッシュデータを生成する．生成されたfem形式のデータは演習のFEMプログラムで利用できる．


利用方法：
stl2fem.exeにSTLファイルをドラッグ・アンド・ドロップ（またはコマンドラインでファイル名を指定）すれば，ファイルと同じ場所に.femと拡張子のついたデータが生成される．コマンドで実行する場合は下記のように書く．

$ stl2fem.exe filename
（filename：ファイルパスを指定）


確認方法：
Windowsであれば付属の実行ファイルが利用できる．sphere.stlファイルをstl2fem.exeにドラッグ・アンド・ドロップし，sphere.stl.femファイルが生成されれば正常に動作している．

（Macやプログラムが実行できない等の場合）コンパイルコマンド：
$ g++ main.cpp mesh.cpp modelhandler.cpp predicates.cpp stlhandler.cpp tetgen.cpp -o stl2fem

生成された実行ファイルで上記の確認を行う．


仕様：
・3Dモデルは左手座標系Y-up（例えばBlenderは右手座標系Z-upなので要注意）
・stlファイルはASCIIでもBinaryでもどちらも対応可能
・メッシュが細かすぎると変換に時間がかかるもしくは変換できない
（頂点数3000を超えると上手く行かない場合が多い）
・モデルの面が閉じていない場合は変換に失敗する
・変換に失敗した場合，temp.stlファイルが残る場合がある（消去可）


参考情報
・TetGen：http://wias-berlin.de/software/tetgen/