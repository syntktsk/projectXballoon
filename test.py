{
 "cells": [
  {
   "cell_type": "code",
   "execution_count": null,
   "id": "7e7c3bc3",
   "metadata": {},
   "outputs": [
    {
     "ename": "",
     "evalue": "",
     "output_type": "error",
     "traceback": [
      "\u001b[1;31m'base (Python 3.12.7)' でセルを実行するには、 ipykernel パッケージが必要です。\n",
      "\u001b[1;31m<a href='command:jupyter.createPythonEnvAndSelectController'>Python 環境の作成</a> および必要なパッケージ。"
     ]
    }
   ],
   "source": [
    "import anl\n",
    "\n",
    "for mod in anl.ModuleRegistry.instance():\n",
    "    print(f\"{mod.name} : {mod.description}\")\n"
   ]
  }
 ],
 "metadata": {
  "kernelspec": {
   "display_name": "base",
   "language": "python",
   "name": "python3"
  },
  "language_info": {
   "name": "python",
   "version": "3.12.7"
  }
 },
 "nbformat": 4,
 "nbformat_minor": 5
}
