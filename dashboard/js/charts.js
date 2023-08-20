document.addEventListener('DOMContentLoaded', function () {
    // Get the current date and the date 3 days ago
    const startDate = new Date('2023-07-17');
    const endDate = new Date('2023-07-26');
  
    // Convert the dates to the format required by the ThingSpeak API (YYYY-MM-DD%20HH:MM:SS)
    const startDateFormatted = startDate.toISOString().slice(0, 19).replace('T', '%20');
    const endDateFormatted = endDate.toISOString().slice(0, 19).replace('T', '%20');
  
    // Fetch data from ThingSpeak API for the specified date range
    fetch(`https://api.thingspeak.com/channels/2219986/feeds.json?api_key=6Z64A9FWXFCNEZ7A&start=${startDateFormatted}&end=${endDateFormatted}`)
      .then(response => response.json())
      .then(data => {
        // Extract data for each chart
        const labels = data.feeds.map(feed => feed.created_at);
        const dataset1 = data.feeds.map(feed => parseFloat(feed.field1));
        const dataset2 = data.feeds.map(feed => parseFloat(feed.field2));
        const dataset3 = data.feeds.map(feed => parseFloat(feed.field3));
  
        // Calculate average date for each range
        const avgDateLabels = [];
        for (let i = 0; i < labels.length - 1; i++) {
          const currentDate = new Date(labels[i]);
          const nextDate = new Date(labels[i + 1]);
          const avgDate = new Date((currentDate.getTime() + nextDate.getTime()) / 2);
          avgDateLabels.push(avgDate.toISOString().slice(0, 10)); // Push YYYY-MM-DD format
        }
  
        // Common options for x-axis configuration
        const options = {
          scales: {
            x: {
              type: 'category',
              labels: avgDateLabels,
              ticks: {
                autoSkip: true,
                maxTicksLimit: 8, // Adjust the number of ticks as needed
              },
              scaleLabel: { // Add a label for the x-axis
                display: true,
                labelString: 'Date',
              },
            },
            y: {
              beginAtZero: true,
            },
          },
        };
  
        // Chart 1
        var ctx1 = document.getElementById('chart1').getContext('2d');
        var chart1 = new Chart(ctx1, {
          type: 'line',
          data: {
            labels: avgDateLabels,
            datasets: [{
              label: 'Temperature 1',
              data: dataset1,
              fill: false,
              borderColor: 'rgb(75, 192, 192)',
              tension: 0.1,
            }],
          },
          options: {
            scales: {
              x: options.scales.x, // Use common x-axis settings
              y: {
                beginAtZero: true,
                scaleLabel: { // Add a label for the y-axis
                  display: true,
                  labelString: 'Temperature in Celsius',
                },
              },
            },
          },
        });
  
        // Chart 2
        var ctx2 = document.getElementById('chart2').getContext('2d');
        var chart2 = new Chart(ctx2, {
          type: 'line',
          data: {
            labels: avgDateLabels,
            datasets: [{
              label: 'Temperature 2',
              data: dataset2,
              fill: false,
              borderColor: 'rgb(255, 99, 132)',
              tension: 0.1,
            }],
          },
          options: {
            scales: {
              x: options.scales.x, // Use common x-axis settings
              y: {
                beginAtZero: true,
                scaleLabel: { // Add a label for the y-axis
                  display: true,
                  labelString: 'Temperature in Celsius',
                },
              },
            },
          },
        });
  
        // Chart 3
        var ctx3 = document.getElementById('chart3').getContext('2d');
        var chart3 = new Chart(ctx3, {
          type: 'line',
          data: {
            labels: avgDateLabels,
            datasets: [{
              label: 'Weight',
              data: dataset3,
              fill: false,
              borderColor: 'rgb(54, 162, 235)',
              tension: 0.1,
            }],
          },
          options: {
            scales: {
              x: options.scales.x, // Use common x-axis settings
              y: {
                beginAtZero: true,
                scaleLabel: { // Add a label for the y-axis
                  display: true,
                  labelString: 'Weight in Kg',
                },
              },
            },
          },
        });
      })
      .catch(error => {
        console.log('Error:', error);
      });
  });
  