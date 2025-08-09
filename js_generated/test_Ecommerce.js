
const request = require('supertest');
const app = require('./index.js');

describe('Application Tests', () => {
    test('Health check endpoint', async () => {
        const response = await request(app)
            .get('/api/health')
            .expect(200);
        
        expect(response.body).toHaveProperty('status', 'OK');
        expect(response.body).toHaveProperty('timestamp');
    });
    
    test('Invalid endpoint returns 404', async () => {
        await request(app)
            .get('/api/nonexistent')
            .expect(404);
    });
});
