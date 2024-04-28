cd ..
bash init.sh
cd scripts
bash run_microbenchmarks.sh
bash run_validity_benchmarks.sh
cd ../data
python energy_pred_model.py